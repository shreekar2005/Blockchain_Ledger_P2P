#include "Node.h"
#include <algorithm>
#include <random>
#include <fstream>

using namespace std;

Define_Module(Node);

void Node::initialize() {
    isSeed = par("isSeed");
    hashPower = par("hashPower");
    myIp = par("myIp").stdstringValue();
    
    keyPair = CryptoUtils::generateKey();
    walletAddress = CryptoUtils::deriveWalletAddress(keyPair);
    
    isSynced = isSeed; 
    gossipCounter = 0;
    plResponsesReceived = 0;
    meanBlockTime = par("meanBlockTime");

    miningTimer = new cMessage("MiningTimer");
    livenessTimer = new cMessage("LivenessTimer");
    txTimer = new cMessage("TxTimer");
    plRetryTimer = new cMessage("PLRetryTimer");
    killTimer = new cMessage("KillTimer");
    invalidTxTimer = new cMessage("InvalidTxTimer");
    doubleSpendTimer = new cMessage("DoubleSpendTimer");
    invalidBlockTimer = new cMessage("InvalidBlockTimer");
    isDead = false;
    triggerInvalidBlock = false;

    double killTimeValue = par("killTime").doubleValue();
    if (killTimeValue > 0) {
        scheduleAt(simTime() + killTimeValue, killTimer);
    }

    double invalidTxValue = par("invalidTxTime").doubleValue();
    if (invalidTxValue > 0) {
        scheduleAt(simTime() + invalidTxValue, invalidTxTimer);
    }

    double doubleSpendValue = par("doubleSpendTime").doubleValue();
    if (doubleSpendValue > 0) {
        scheduleAt(simTime() + doubleSpendValue, doubleSpendTimer);
    }

    double invalidBlockValue = par("invalidBlockTime").doubleValue();
    if (invalidBlockValue > 0) {
        scheduleAt(simTime() + invalidBlockValue, invalidBlockTimer);
    }
    
    ledger.createGenesis();

    if (getParentModule()->hasPar("numSeeds")) {
        totalSeedsInNetwork = getParentModule()->par("numSeeds").intValue();
    } else {
        totalSeedsInNetwork = 5;
    }

    if (!isSeed) {
        ifstream seedFile("seed_ips.txt");
        string ipLine;
        while (getline(seedFile, ipLine)) {
            if (!ipLine.empty()) {
                if (ipLine.back() == '\r') ipLine.pop_back(); 
                allSeedIps.push_back(ipLine);
            }
        }
        
        numSeedsToRegister = static_cast<int>(floor(allSeedIps.size() / 2.0)) + 1;
    }

    scheduleAt(simTime() + par("startUpTime").doubleValue(), new cMessage("StartUp"));
}

void Node::connectToNode(cModule* targetNode) {
    int currentGateSize = gateSize("port");
    int targetGateSize = targetNode->gateSize("port");
    
    setGateSize("port", currentGateSize + 1);
    targetNode->setGateSize("port", targetGateSize + 1);
    
    cDelayChannel *outgoingChannel = cDelayChannel::create("channel");
    outgoingChannel->setDelay(0.1); 
    gate("port$o", currentGateSize)->connectTo(targetNode->gate("port$i", targetGateSize), outgoingChannel);
    
    cDelayChannel *incomingChannel = cDelayChannel::create("channel");
    incomingChannel->setDelay(0.1);
    targetNode->gate("port$o", targetGateSize)->connectTo(gate("port$i", currentGateSize), incomingChannel);
}

void Node::handleMessage(cMessage *msg) {
    if (msg == killTimer) {
        isDead = true;
        EV << "NODE FAILURE SIMULATED: I am now dead." << endl;
        if (miningTimer->isScheduled()) cancelEvent(miningTimer);
        if (livenessTimer->isScheduled()) cancelEvent(livenessTimer);
        if (txTimer->isScheduled()) cancelEvent(txTimer);
        if (plRetryTimer->isScheduled()) cancelEvent(plRetryTimer);
        return; 
    }
    
    if (msg == invalidTxTimer) {
        TransactionMsg tx;
        tx.setSenderAddress(walletAddress.c_str());
        tx.setSenderPublicKey(CryptoUtils::getPublicKeyString(keyPair).c_str());
        tx.setData("MALICIOUS: Unauthorized Transfer");
        tx.setTimestamp(simTime().dbl());
        tx.setTxId(CryptoUtils::sha256("malicious_data").c_str());
        tx.setSigR("INVALID_SIGNATURE_PART_R");
        tx.setSigS("INVALID_SIGNATURE_PART_S");
        
        GossipMsg *gossip = new GossipMsg("InvalidTx");
        gossip->setType(2); 
        gossip->setOriginatorIp(myIp.c_str());
        gossip->setMsgId(gossipCounter++);
        
        string identifier = to_string(simTime().dbl()) + ":" + myIp + ":" + to_string(gossip->getMsgId());
        gossip->setGossipFormat(identifier.c_str());
        gossip->setTxPayload(tx);
        
        broadcast(gossip);
        EV << "MALICIOUS ACTION: Generated Invalid Transaction [ID: " << string(tx.getTxId()).substr(0, 8) << "...]" << endl;
        return;
    }

    if (msg == doubleSpendTimer) {
        TransactionMsg tx1, tx2;
        tx1.setSenderAddress(walletAddress.c_str());
        tx1.setSenderPublicKey(CryptoUtils::getPublicKeyString(keyPair).c_str());
        tx1.setData("SPEND:Initial_Supply");
        tx1.setTimestamp(simTime().dbl());
        tx1.setTxId(CryptoUtils::sha256("ds_tx_1").c_str());
        
        string r, s;
        CryptoUtils::signData(tx1.getData(), keyPair, r, s);
        tx1.setSigR(r.c_str());
        tx1.setSigS(s.c_str());

        tx2 = tx1;
        tx2.setTxId(CryptoUtils::sha256(to_string(simTime().dbl()) + "_ds_tx_2").c_str());
        tx2.setTimestamp(simTime().dbl() + 0.001);

        GossipMsg *g1 = new GossipMsg("DoubleSpend1");
        g1->setType(2);
        g1->setTxPayload(tx1);
        g1->setGossipFormat((to_string(simTime().dbl()) + ":" + myIp + ":DS1").c_str());
        
        GossipMsg *g2 = new GossipMsg("DoubleSpend2");
        g2->setType(2);
        g2->setTxPayload(tx2);
        g2->setGossipFormat((to_string(simTime().dbl()) + ":" + myIp + ":DS2").c_str());
        
        broadcast(g1);
        broadcast(g2);
        EV << "MALICIOUS ACTION: Generated Double Spend Txs: [ID: " << string(tx1.getTxId()).substr(0, 8) << "] and [ID: " << string(tx2.getTxId()).substr(0, 8) << "]" << endl;
        return;
    }

    if (msg == invalidBlockTimer) {
        EV << "MALICIOUS ACTION: Node will mine an INVALID block NOW for demo." << endl;
        if (miningTimer->isScheduled()) cancelEvent(miningTimer);
        triggerInvalidBlock = true;
        scheduleAt(simTime(), miningTimer);
        return;
    }

    if (isDead) {
        if (msg != miningTimer && msg != livenessTimer && msg != txTimer && 
            msg != plRetryTimer && msg != invalidTxTimer && msg != doubleSpendTimer && 
            msg != invalidBlockTimer) {
            delete msg;
        }
        return;
    }

    if (strcmp(msg->getName(), "StartUp") == 0) {
        if (!isSeed) initiatePeerRegistration();
        scheduleAt(simTime() + 1.0, livenessTimer);
        delete msg;
        return;
    }

    if (msg == plRetryTimer) {
        initiatePeerRegistration();
        return;
    }

    if (msg == livenessTimer) {
        sendLivenessPing();
        checkLiveness();
        scheduleAt(simTime() + 13.0, livenessTimer);
        return;
    }

    if (msg == txTimer) {
        generateTransaction();
        scheduleAt(simTime() + uniform(15.0, 30.0), txTimer);
        return;
    }

    if (msg == miningTimer) {
        LocalBlock candidate = ledger.createCandidateBlock(getId(), myIp, keyPair, walletAddress, mempool);
        BlockMsg *blockMsg = new BlockMsg("BlockData");
        
        string tag = triggerInvalidBlock ? " (MALICIOUS)" : "";
        if (triggerInvalidBlock) {
            EV << "MALICIOUS ACTION: Mining an INVALID block (Fake Previous Hash)..." << endl;
            blockMsg->setPreviousHash("INVALID_CHAIN_LINK_0000");
            triggerInvalidBlock = false; 
        } else {
            blockMsg->setPreviousHash(candidate.prevHash.c_str());
        }

        blockMsg->setIndex(candidate.index);
        blockMsg->setCurrentHash(candidate.hash.c_str());
        blockMsg->setMerkleRoot(candidate.merkleRoot.c_str());
        blockMsg->setTimestamp(simTime().dbl());
        blockMsg->setMinerId(myIp.c_str());
        
        size_t txCount = candidate.transactions.size();
        blockMsg->setTransactionsArraySize(txCount);
        string txList = "";
        for (size_t i = 0; i < txCount; ++i) {
            blockMsg->setTransactions(i, candidate.transactions[i]);
            txList += string(candidate.transactions[i].getTxId()).substr(0, 8) + (i == txCount - 1 ? "" : ", ");
        }

        ledger.validateAndAddBlock(blockMsg, simTime().dbl()); 
        mempool.clear(); 
        
        EV << "BLOCK MINED" << tag << ": Index=" << candidate.index << " | Hash=" << string(candidate.hash).substr(0, 8) << "... | Txs=[" << txList << "] | Miner=" << myIp << endl;
        broadcast(blockMsg->dup());
        delete blockMsg;
        startMining();
        return;
    }

    if (dynamic_cast<RegisterMsg*>(msg)) {
        if (isSeed) {
            RegisterMsg* regMsg = static_cast<RegisterMsg*>(msg);
            knownPeerIds.insert(regMsg->getPeerModuleId());
            
            PeerListMsg* peerList = new PeerListMsg("PeerList");
            size_t peerCount = knownPeerIds.size();
            peerList->setPeersArraySize(peerCount);
            peerList->setPeerModuleIdsArraySize(peerCount);
            
            int index = 0;
            for (int peerId : knownPeerIds) {
                peerList->setPeerModuleIds(index++, peerId);
            }
            send(peerList, "port$o", msg->getArrivalGate()->getIndex());
        }
        delete msg;
        return;
    }

    if (dynamic_cast<PeerListMsg*>(msg)) {
        if (!isSeed && activeConnections.size() < 4) {
            processPeerList(static_cast<PeerListMsg*>(msg));
        }
        delete msg;
        return;
    }

    GossipMsg *gossipMsg = dynamic_cast<GossipMsg*>(msg);
    if (gossipMsg) {
        if (gossipMsg->getType() == 3) { 
            lastHeardFrom[msg->getArrivalGate()->getIndex()] = simTime();
            delete msg;
            return;
        }
        
        if (gossipMsg->getType() == 4) {
            string payload = gossipMsg->getPayload();
            string gossipId = "DEAD_NODE_SIGNAL_" + payload; 
            
            if (messageList.find(gossipId) == messageList.end()) {
                messageList.insert(gossipId);
                
                if (isSeed) {
                    size_t start = payload.find(':') + 1;
                    size_t end = payload.find(':', start);
                    string deadIp = payload.substr(start, end - start);
                    
                    int targetId = -1;
                    for (int id : knownPeerIds) {
                        cModule* mod = getSimulation()->getModule(id);
                        if (mod && mod->hasPar("myIp") && mod->par("myIp").stdstringValue() == deadIp) {
                            targetId = id;
                            break;
                        }
                    }
                    if (targetId != -1) {
                        knownPeerIds.erase(targetId);
                        EV << "FINAL DECLARATION: Node " << deadIp << " is confirmed DEAD. Peer List updated." << endl;
                    }
                }
                broadcast(gossipMsg->dup());
            }
            delete msg;
            return;
        }        

        string gossipFormatId = gossipMsg->getGossipFormat();
        if (messageList.find(gossipFormatId) == messageList.end()) {
            messageList.insert(gossipFormatId);
            
            if (gossipMsg->getType() == 2 && !isSeed) {
                TransactionMsg incomingTx = gossipMsg->getTxPayload();
                string tid = string(incomingTx.getTxId()).substr(0, 8);
                
                if (ledger.validateTransaction(incomingTx)) {
                    bool isDoubleSpendInMempool = false;
                    string incomingData = incomingTx.getData();
                    
                    if (incomingData.rfind("SPEND:", 0) == 0) {
                        string incomingAsset = incomingData.substr(6);
                        for (const auto& tx : mempool) {
                            string existingData = tx.getData();
                            if (existingData.rfind("SPEND:", 0) == 0 && existingData.substr(6) == incomingAsset) {
                                isDoubleSpendInMempool = true;
                                break;
                            }
                        }
                    }

                    if (!isDoubleSpendInMempool) {
                        mempool.push_back(incomingTx);
                        EV << "Valid transaction [ID: " << tid << "...] received and added to mempool: " << incomingTx.getData() << endl;
                    } else {
                        EV << "DOUBLE SPEND REJECTED [ID: " << tid << "...]: Asset already has a pending spend in mempool." << endl;
                    }
                } else {
                    EV << "INVALID TRANSACTION [ID: " << tid << "...] REJECTED: Signature verification failed or invalid asset." << endl;
                }
            }
            broadcast(gossipMsg->dup());
        }
        delete msg;
        return;
    }

    BlockMsg *blockMsg = dynamic_cast<BlockMsg*>(msg);
    if (blockMsg) {
        handleSync(blockMsg);
        delete msg;
    }
}

void Node::generateTransaction() {
    TransactionMsg tx;
    tx.setSenderAddress(walletAddress.c_str());
    tx.setSenderPublicKey(CryptoUtils::getPublicKeyString(keyPair).c_str());

    string receiverAddr = "0x0000";
    if (!knownPeerIds.empty()) {
        auto it = knownPeerIds.begin();
        advance(it, intrand(knownPeerIds.size()));
        cModule* receiverMod = getSimulation()->getModule(*it);
        if (receiverMod && receiverMod->hasPar("myIp")) {
            receiverAddr = "0x" + CryptoUtils::sha256(receiverMod->par("myIp").stdstringValue()).substr(0, 4);
        }
    }
    tx.setReceiverAddress(receiverAddr.c_str());
    tx.setData(("Transfer: Energy Credits from " + myIp).c_str());
    tx.setTimestamp(simTime().dbl());

    string txRawContent = string(tx.getSenderAddress()) + 
                               string(tx.getReceiverAddress()) + 
                               string(tx.getData()) + 
                               to_string(tx.getTimestamp());
    tx.setTxId(CryptoUtils::sha256(txRawContent).c_str());

    string r, s;
    CryptoUtils::signData(tx.getData(), keyPair, r, s);
    tx.setSigR(r.c_str());
    tx.setSigS(s.c_str());

    mempool.push_back(tx); 

    GossipMsg *gossip = new GossipMsg("NewTx");
    gossip->setType(2);
    gossip->setOriginatorIp(myIp.c_str());
    gossip->setMsgId(gossipCounter++);
    
    string identifier = to_string(simTime().dbl()) + ":" + myIp + ":" + to_string(gossip->getMsgId());
    gossip->setGossipFormat(identifier.c_str());
    gossip->setTxPayload(tx);

    broadcast(gossip);
    EV << "GOSSIP FORMAT: " << identifier << " | New Transaction Generated [ID: " << string(tx.getTxId()).substr(0, 8) << "...]" << endl;
}

void Node::initiatePeerRegistration() {
    int connectionCount = 0;
    cModule* network = getParentModule();
    for (const string& targetSeedIp : allSeedIps) {
        if (connectionCount >= numSeedsToRegister) break;
        
        cModule* seedModule = nullptr;
        for (cModule::SubmoduleIterator it(network); !it.end(); ++it) {
            cModule *submod = *it;
            if (submod->hasPar("myIp") && submod->par("myIp").stdstringValue() == targetSeedIp) {
                seedModule = submod;
                break;
            }
        }
        
        if (seedModule) {
            connectToNode(seedModule);
            connectedSeedIds.insert(seedModule->getId());
            
            RegisterMsg* regMsg = new RegisterMsg("Register");
            regMsg->setPeerIp(myIp.c_str());
            regMsg->setPeerModuleId(getId());
            send(regMsg, "port$o", gateSize("port") - 1);
            connectionCount++;
        }
    }
}

void Node::processPeerList(PeerListMsg* msg) {
    plResponsesReceived++;
    
    size_t peerArraySize = msg->getPeerModuleIdsArraySize();
    for (size_t i = 0; i < peerArraySize; ++i) {
        int peerId = msg->getPeerModuleIds(i);
        if (peerId != getId()) {
            knownPeerIds.insert(peerId);
        }
    }

    if (plResponsesReceived >= numSeedsToRegister) {
        establishTCPConnections();
    }
}

void Node::establishTCPConnections() {
    vector<int> candidates;
    for (int id : knownPeerIds) {
        if (connectedSeedIds.find(id) == connectedSeedIds.end()) {
            candidates.push_back(id);
        }
    }
    
    random_device rd;
    mt19937 g(rd());
    shuffle(candidates.begin(), candidates.end(), g);
    
    size_t neededCount = 4 - activeConnections.size();
    size_t establishedCount = 0;
    
    for (int candidateId : candidates) {
        if (establishedCount >= neededCount) break;
        
        auto it = find(activeConnections.begin(), activeConnections.end(), candidateId);
        if (it != activeConnections.end()) continue;
        
        cModule* peerMod = getSimulation()->getModule(candidateId);
        if (peerMod) {
            connectToNode(peerMod);
            activeConnections.push_back(candidateId);
            establishedCount++;
        }
    }
    
    EV << "P2P Union Rule: Selected " << activeConnections.size() << " distinct peers." << endl;
    
    if (activeConnections.size() < 4) {
        if (plRetryTimer->isScheduled()) cancelEvent(plRetryTimer);
        scheduleAt(simTime() + 10.0, plRetryTimer);
    }

    if (!isSynced && activeConnections.size() >= 4) {
        isSynced = true; 
        scheduleAt(simTime() + uniform(5.0, 10.0), txTimer);
        startMining();
    }
}

void Node::handleSync(BlockMsg* bMsg) {
    if (!isSynced) {
        if (bMsg->getIndex() > ledger.getHeight()) {
            pendingQueue[bMsg->getIndex()] = bMsg->dup();
            isSynced = true; 
            processPendingQueue();
        }
        return;
    }

    if (miningTimer->isScheduled()) cancelEvent(miningTimer);
    
    int validationResult = ledger.validateAndAddBlock(bMsg, simTime().dbl());
    string bHash = string(bMsg->getCurrentHash()).substr(0, 8);
    int txCount = bMsg->getTransactionsArraySize();
    if (validationResult == 1) {
        EV << "BLOCK ACCEPTED: Index=" << bMsg->getIndex() << " | Hash=" << bHash << "... | Txs=" << txCount << " | From=" << bMsg->getMinerId() << endl;
        cleanMempool(bMsg); 
        broadcast(bMsg->dup());
        if (pendingQueue.empty()) startMining();
    } else if (validationResult == -1) {
        EV << "MALICIOUS BLOCK REJECTED: Index=" << bMsg->getIndex() << " | Hash=" << bHash << "... | Txs=" << txCount << " | Reason: Validation failed." << endl;
    }
}

void Node::processPendingQueue() {
    auto it = pendingQueue.begin();
    while (it != pendingQueue.end()) {
        if (ledger.validateAndAddBlock(it->second, simTime().dbl()) == 1) {
            cleanMempool(it->second);
            delete it->second;
            it = pendingQueue.erase(it);
        } else {
            ++it;
        }
    }
    if (pendingQueue.empty()) startMining();
}

void Node::cleanMempool(BlockMsg* bMsg) {
    size_t txCount = bMsg->getTransactionsArraySize();
    for (size_t i = 0; i < txCount; ++i) {
        string confirmedData = bMsg->getTransactions(i).getData();
        mempool.erase(
            remove_if(mempool.begin(), mempool.end(),
                [&](const TransactionMsg& t) { 
                    return string(t.getData()) == confirmedData; 
                }),
            mempool.end()
        );
    }
}

void Node::startMining() {
    if (isSeed || !pendingQueue.empty()) return; 
    if (miningTimer->isScheduled()) cancelEvent(miningTimer);
    
    double lambdaFactor = (hashPower * (1.0 / meanBlockTime)) / 100.0;
    scheduleAt(simTime() + exponential(1.0 / lambdaFactor), miningTimer);
}

void Node::sendLivenessPing() {
    GossipMsg *ping = new GossipMsg("Liveness");
    ping->setType(3);
    
    string identifier = to_string(simTime().dbl()) + ":" + myIp + ":" + to_string(gossipCounter++);
    ping->setGossipFormat(identifier.c_str());
    
    broadcast(ping); 
}

void Node::checkLiveness() {
    int totalGates = gateSize("port");
    for (int i = 0; i < totalGates; ++i) {
        cGate* outGate = gate("port$o", i);
        if (!outGate->isConnected()) continue;
        
        if (simTime().dbl() - lastHeardFrom[i] >= 39.0) {
            cModule* remoteModule = outGate->getPathEndGate()->getOwnerModule();
            string remoteIp = remoteModule->hasPar("myIp") ? remoteModule->par("myIp").stdstringValue() : "Unknown";

            string failureSignal = "Dead Node:" + remoteIp + ":" + to_string(i) + ":" + to_string(simTime().dbl()) + ":" + myIp;
            
            GossipMsg *alert = new GossipMsg("DeadNodeAlert");
            alert->setType(4);
            alert->setPayload(failureSignal.c_str());
            
            broadcast(alert, true); 
            outGate->disconnect();
            EV << "REPORTED DEAD NODE: " << failureSignal << endl;
        }
    }
}

void Node::broadcast(cMessage *msg, bool toSeedsOnly) {
    int totalGates = gateSize("port");
    for (int i = 0; i < totalGates; ++i) {
        cGate* outGate = gate("port$o", i);
        if (outGate->isConnected()) {
            cModule* neighbor = outGate->getPathEndGate()->getOwnerModule();
            bool isSeedNeighbor = (connectedSeedIds.find(neighbor->getId()) != connectedSeedIds.end());
            
            if (toSeedsOnly && !isSeedNeighbor) continue;
            send(msg->dup(), "port$o", i);
        }
    }
    delete msg; 
}

void Node::finish() { 
    if (keyPair) EVP_PKEY_free(keyPair);
    
    for (auto& entry : pendingQueue) {
        delete entry.second;
    }
    pendingQueue.clear();

    cancelAndDelete(miningTimer);
    cancelAndDelete(livenessTimer);
    cancelAndDelete(txTimer);
    cancelAndDelete(plRetryTimer);
    cancelAndDelete(killTimer);
    cancelAndDelete(invalidTxTimer);
    cancelAndDelete(doubleSpendTimer);
    cancelAndDelete(invalidBlockTimer);
}
