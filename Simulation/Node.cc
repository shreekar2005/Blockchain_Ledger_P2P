#include "Node.h"
#include <algorithm>
#include <random>
#include <fstream>

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

    double kt = par("killTime").doubleValue();
    if (kt > 0) scheduleAt(simTime() + kt, killTimer);

    double itx = par("invalidTxTime").doubleValue();
    if (itx > 0) scheduleAt(simTime() + itx, invalidTxTimer);

    double dst = par("doubleSpendTime").doubleValue();
    if (dst > 0) scheduleAt(simTime() + dst, doubleSpendTimer);

    double ibt = par("invalidBlockTime").doubleValue();
    if (ibt > 0) scheduleAt(simTime() + ibt, invalidBlockTimer);
    
    ledger.createGenesis();

    if (getParentModule()->hasPar("numSeeds")) {
        totalSeedsInNetwork = getParentModule()->par("numSeeds").intValue();
    } else { totalSeedsInNetwork = 5; }

    if (!isSeed) {
        std::ifstream infile("seed_ips.txt");
        std::string ip;
        while (std::getline(infile, ip)) {
            if (!ip.empty()) {
                if (ip.back() == '\r') ip.pop_back(); 
                allSeedIps.push_back(ip);
            }
        }
        if (allSeedIps.empty()) EV << "ERROR: seed_ips.txt is empty or missing!" << endl;
        
        numSeedsToRegister = std::floor(allSeedIps.size() / 2.0) + 1;
    }

    scheduleAt(simTime() + par("startUpTime").doubleValue(), new cMessage("StartUp"));
}

void Node::connectToNode(cModule* targetNode) {
    int myGateSize = gateSize("port");
    int targetGateSize = targetNode->gateSize("port");
    setGateSize("port", myGateSize + 1);
    targetNode->setGateSize("port", targetGateSize + 1);
    
    cDelayChannel *txChannel = cDelayChannel::create("channel");
    txChannel->setDelay(0.1); 
    gate("port$o", myGateSize)->connectTo(targetNode->gate("port$i", targetGateSize), txChannel);
    
    cDelayChannel *rxChannel = cDelayChannel::create("channel");
    rxChannel->setDelay(0.1);
    targetNode->gate("port$o", targetGateSize)->connectTo(gate("port$i", myGateSize), rxChannel);
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
        EV << "MALICIOUS ACTION: Generating Invalid Transaction (Bad Signature)..." << endl;
        TransactionMsg tx;
        tx.setSenderAddress(walletAddress.c_str());
        tx.setSenderPublicKey(CryptoUtils::getPublicKeyString(keyPair).c_str());
        tx.setData("MALICIOUS: Attempting to steal oil.");
        tx.setTimestamp(simTime().dbl());
        tx.setTxId(CryptoUtils::sha256("malicious").c_str());
        tx.setSigR("BAD_SIG_R"); // Corrupting signature
        tx.setSigS("BAD_SIG_S");
        
        std::string tid = std::string(tx.getTxId()).substr(0,8);
        GossipMsg *gMsg = new GossipMsg("InvalidTx");
        gMsg->setType(2); 
        gMsg->setOriginatorIp(myIp.c_str());
        gMsg->setMsgId(gossipCounter++);
        std::string format = std::to_string(simTime().dbl()) + ":" + myIp + ":" + std::to_string(gMsg->getMsgId());
        gMsg->setGossipFormat(format.c_str());
        gMsg->setTxPayload(tx);
        broadcast(gMsg);
        EV << "MALICIOUS ACTION: Generated Invalid Transaction [ID: " << tid << "...]" << endl;
        return;
    }

    if (msg == doubleSpendTimer) {
        EV << "MALICIOUS ACTION: Attempting Double Spend (Two txs same asset)..." << endl;
        TransactionMsg tx1, tx2;
        tx1.setSenderAddress(walletAddress.c_str());
        tx1.setSenderPublicKey(CryptoUtils::getPublicKeyString(keyPair).c_str());
        tx1.setData("SPEND:Oil_Batch_0"); // Genesis/early asset
        tx1.setTimestamp(simTime().dbl());
        tx1.setTxId(CryptoUtils::sha256("ds1").c_str());
        std::string r1, s1; CryptoUtils::signData(tx1.getData(), keyPair, r1, s1);
        tx1.setSigR(r1.c_str()); tx1.setSigS(s1.c_str());

        tx2 = tx1; // Duplicate everything
        tx2.setTxId(CryptoUtils::sha256(std::to_string(simTime().dbl()) + "ds2").c_str()); // Unique TxID but same asset/data
        tx2.setTimestamp(simTime().dbl() + 0.001);

        GossipMsg *g1 = new GossipMsg("DoubleSpend1");
        g1->setType(2); g1->setTxPayload(tx1);
        g1->setGossipFormat((std::to_string(simTime().dbl()) + ":" + myIp + ":DS1").c_str());
        
        GossipMsg *g2 = new GossipMsg("DoubleSpend2");
        g2->setType(2); g2->setTxPayload(tx2);
        g2->setGossipFormat((std::to_string(simTime().dbl()) + ":" + myIp + ":DS2").c_str());
        
        broadcast(g1); broadcast(g2);
        std::string tid1 = std::string(tx1.getTxId()).substr(0,8);
        std::string tid2 = std::string(tx2.getTxId()).substr(0,8);
        EV << "MALICIOUS ACTION: Generated Double Spend Txs: [ID: " << tid1 << "...] and [ID: " << tid2 << "...]" << endl;
        return;
    }

    if (msg == invalidBlockTimer) {
        EV << "MALICIOUS ACTION: Node will mine an INVALID block NOW for demo." << endl;
        if (miningTimer->isScheduled()) cancelEvent(miningTimer);
        triggerInvalidBlock = true;
        scheduleAt(simTime(), miningTimer); // Force mining NOW
        return;
    }

    if (isDead) {
        if (msg == miningTimer || msg == livenessTimer || msg == txTimer || msg == plRetryTimer || msg == invalidTxTimer || msg == doubleSpendTimer || msg == invalidBlockTimer) {
            return; 
        }
        delete msg;
        return;
    }

    if (strcmp(msg->getName(), "StartUp") == 0) {
        if (!isSeed) initiatePeerRegistration();
        scheduleAt(simTime() + 1.0, livenessTimer);
        delete msg; return;
    }

    if (msg == plRetryTimer) {
        initiatePeerRegistration(); // Request new list
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
        scheduleAt(simTime() + uniform(15.0, 30.0), txTimer); // Generate new events periodically
        return;
    }

    if (msg == miningTimer) {
        LocalBlock b = ledger.createCandidateBlock(getId(), myIp, keyPair, walletAddress, mempool);
        BlockMsg *bMsg = new BlockMsg("BlockData");
        
        std::string bh = std::string(b.hash).substr(0,8);
        std::string txList = "";
        for(size_t k=0; k<b.transactions.size(); k++) {
            txList += std::string(b.transactions[k].getTxId()).substr(0,8) + (k == b.transactions.size()-1 ? "" : ", ");
        }
        
        std::string tag = triggerInvalidBlock ? " (MALICIOUS)" : "";
        if (triggerInvalidBlock) {
            EV << "MALICIOUS ACTION: Mining an INVALID block (Fake Previous Hash)..." << endl;
            bMsg->setPreviousHash("FAKE_PREV_HASH");
            triggerInvalidBlock = false; 
        } else {
            bMsg->setPreviousHash(b.prevHash.c_str());
        }

        bMsg->setIndex(b.index);
        bMsg->setCurrentHash(b.hash.c_str());
        bMsg->setMerkleRoot(b.merkleRoot.c_str());
        bMsg->setTimestamp(simTime().dbl());
        bMsg->setMinerId(myIp.c_str());
        
        bMsg->setTransactionsArraySize(b.transactions.size());
        for(size_t k=0; k<b.transactions.size(); k++) bMsg->setTransactions(k, b.transactions[k]);

        ledger.validateAndAddBlock(bMsg, simTime().dbl()); 
        mempool.clear(); 
        
        EV << "BLOCK MINED" << tag << ": Index=" << b.index << " | Hash=" << bh << "... | Txs=[" << txList << "] | Miner=" << myIp << endl;
        broadcast(bMsg->dup());
        delete bMsg;
        startMining();
        return;
    }

    // P2P Registration
    if (dynamic_cast<RegisterMsg*>(msg)) {
        if (isSeed) {
            RegisterMsg* rMsg = (RegisterMsg*)msg;
            knownPeerIds.insert(rMsg->getPeerModuleId());
            
            PeerListMsg* pList = new PeerListMsg("PeerList");
            pList->setPeersArraySize(knownPeerIds.size());
            pList->setPeerModuleIdsArraySize(knownPeerIds.size());
            
            int i = 0;
            for(int peerId : knownPeerIds) {
                pList->setPeerModuleIds(i++, peerId);
            }
            send(pList, "port$o", msg->getArrivalGate()->getIndex());
        }
        delete msg; return;
    }

    if (dynamic_cast<PeerListMsg*>(msg)) {
        if (!isSeed && activeConnections.size() < 4) processPeerList((PeerListMsg*)msg);
        delete msg; return;
    }

    // Gossip Protocol [cite: 25]
    GossipMsg *gMsg = dynamic_cast<GossipMsg*>(msg);
    if (gMsg) {
        if (gMsg->getType() == 3) { 
            lastHeardFrom[msg->getArrivalGate()->getIndex()] = simTime();
            delete msg; return;
        }
        if (gMsg->getType() == 4) {
            std::string payload = gMsg->getPayload();
            std::string gossipId = "DEAD_NODE_" + payload; 
            
            if (messageList.find(gossipId) == messageList.end()) {
                messageList.insert(gossipId);
                
                if (isSeed) {
                    size_t firstColon = payload.find(':');
                    size_t secondColon = payload.find(':', firstColon + 1);
                    std::string deadIp = payload.substr(firstColon + 1, secondColon - firstColon - 1);
                    
                    int deadId = -1;
                    for (int id : knownPeerIds) {
                        cModule* mod = getSimulation()->getModule(id);
                        if (mod && mod->hasPar("myIp") && mod->par("myIp").stdstringValue() == deadIp) {
                            deadId = id; break;
                        }
                    }
                    if (deadId != -1) {
                        knownPeerIds.erase(deadId);
                        EV << "FINAL DECLARATION: Node " << deadIp << " is confirmed DEAD. Peer List updated." << endl;
                    }
                }
                broadcast(gMsg->dup()); // Gossip it!
            }
            delete msg; return;
        }        
        // Check Message List (ML) [cite: 31, 32]
        std::string gossipId = gMsg->getGossipFormat();
        if (messageList.find(gossipId) == messageList.end()) {
            messageList.insert(gossipId);
            
            // If it's a Transaction Gossip (Type 2), validate and add to mempool
            if (gMsg->getType() == 2 && !isSeed) {
                TransactionMsg incomingTx = gMsg->getTxPayload();
                std::string tid = std::string(incomingTx.getTxId()).substr(0,8);
                if (ledger.validateTransaction(incomingTx)) {
                    mempool.push_back(incomingTx);
                    EV << "Valid transaction [ID: " << tid << "...] received and added to mempool: " << incomingTx.getData() << endl;
                } else {
                    EV << "INVALID TRANSACTION [ID: " << tid << "...] REJECTED: Signature verification failed or invalid asset." << endl;
                }
            }
            broadcast(gMsg->dup());
        }
        delete msg; return;
    }

    BlockMsg *bMsg = dynamic_cast<BlockMsg*>(msg);
    if (bMsg) { handleSync(bMsg); delete msg; }
}

void Node::generateTransaction() {
    TransactionMsg tx;
    tx.setSenderAddress(walletAddress.c_str());
    tx.setSenderPublicKey(CryptoUtils::getPublicKeyString(keyPair).c_str());

    std::string receiver = "0x0000";
    if (!knownPeerIds.empty()) {
        auto it = knownPeerIds.begin();
        std::advance(it, intrand(knownPeerIds.size()));
        cModule* recvMod = getSimulation()->getModule(*it);
        if (recvMod && recvMod->hasPar("myIp")) {
            receiver = "0x" + CryptoUtils::sha256(recvMod->par("myIp").stdstringValue()).substr(0,4);
        }
    }
    tx.setReceiverAddress(receiver.c_str());
    tx.setData(("Petroleum Update: 100 barrels from " + myIp).c_str());
    tx.setTimestamp(simTime().dbl());

    std::string content = std::string(tx.getSenderAddress()) + 
                          std::string(tx.getReceiverAddress()) + 
                          std::string(tx.getData()) + 
                          std::to_string(tx.getTimestamp());
    tx.setTxId(CryptoUtils::sha256(content).c_str());

    std::string r, s;
    CryptoUtils::signData(tx.getData(), keyPair, r, s);
    tx.setSigR(r.c_str());
    tx.setSigS(s.c_str());

    mempool.push_back(tx); 

    GossipMsg *gMsg = new GossipMsg("NewTx");
    gMsg->setType(2);
    gMsg->setOriginatorIp(myIp.c_str());
    gMsg->setMsgId(gossipCounter++);
    std::string format = std::to_string(simTime().dbl()) + ":" + myIp + ":" + std::to_string(gMsg->getMsgId());
    gMsg->setGossipFormat(format.c_str());
    gMsg->setTxPayload(tx);

    broadcast(gMsg);
    std::string tid = std::string(tx.getTxId()).substr(0,8);
    EV << "GOSSIP FORMAT: " << format << " | New Transaction Generated [ID: " << tid << "...]" << endl;
}
void Node::initiatePeerRegistration() {
    int connectionsMade = 0;
    cModule* parent = getParentModule();
    for (const std::string& targetIp : allSeedIps) {
        if (connectionsMade >= numSeedsToRegister) break;
        cModule* targetSeed = nullptr;
        for (cModule::SubmoduleIterator it(parent); !it.end(); ++it) {
            cModule *submod = *it;
            if (submod->hasPar("myIp") && submod->par("myIp").stdstringValue() == targetIp) {
                targetSeed = submod; break;
            }
        }
        if (targetSeed) {
            connectToNode(targetSeed);
            connectedSeedIds.insert(targetSeed->getId());
            RegisterMsg* rMsg = new RegisterMsg("Register");
            rMsg->setPeerIp(myIp.c_str());
            rMsg->setPeerModuleId(getId());
            send(rMsg, "port$o", gateSize("port") - 1);
            connectionsMade++;
        }
    }
}

void Node::processPeerList(PeerListMsg* msg) {
    plResponsesReceived++;
    
    // Union Rule: Add all received peer IDs to the set (automatically handles duplicates)
    for(size_t i=0; i<msg->getPeerModuleIdsArraySize(); i++) {
        int id = msg->getPeerModuleIds(i);
        // Filter out self
        if (id != getId()) {
            knownPeerIds.insert(id);
        }
    }

    // Wait for all seed responses before establishing connections
    if (plResponsesReceived >= numSeedsToRegister) {
        establishTCPConnections();
    }
}

void Node::establishTCPConnections() {
    std::vector<int> candidates;
    for (int id : knownPeerIds) {
        if (connectedSeedIds.find(id) == connectedSeedIds.end()) candidates.push_back(id);
    }
    std::shuffle(candidates.begin(), candidates.end(), std::default_random_engine(std::random_device{}()));
    
    int needed = 4 - activeConnections.size();
    int added = 0;
    for (int targetId : candidates) {
        if (added >= needed) break;
        bool alreadyConnected = false;
        for(int conn : activeConnections) if(conn == targetId) alreadyConnected = true;
        if (alreadyConnected) continue;
        
        cModule* peerModule = getSimulation()->getModule(targetId);
        if (peerModule) {
            connectToNode(peerModule);
            activeConnections.push_back(targetId);
            added++;
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
    int result = ledger.validateAndAddBlock(bMsg, simTime().dbl());
    std::string bHash = std::string(bMsg->getCurrentHash()).substr(0,8);
    int txCount = bMsg->getTransactionsArraySize();
    if (result == 1) {
        EV << "BLOCK ACCEPTED: Index=" << bMsg->getIndex() << " | Hash=" << bHash << "... | Txs=" << txCount << " | From=" << bMsg->getMinerId() << endl;
        cleanMempool(bMsg); 
        broadcast(bMsg->dup());
        if (pendingQueue.empty()) startMining();
    } else if (result == -1) {
        EV << "MALICIOUS BLOCK REJECTED: Index=" << bMsg->getIndex() << " | Hash=" << bHash << "... | Txs=" << txCount << " | Reason: Validation failed." << endl;
    }
}

void Node::processPendingQueue() {
    for (auto it = pendingQueue.begin(); it != pendingQueue.end(); ) {
        if (ledger.validateAndAddBlock(it->second, simTime().dbl()) == 1) {
            cleanMempool(it->second);
            delete it->second;
            it = pendingQueue.erase(it);
        } else { ++it; }
    }
    if (pendingQueue.empty()) startMining();
}

void Node::cleanMempool(BlockMsg* bMsg) {
    for (size_t k = 0; k < bMsg->getTransactionsArraySize(); k++) {
        std::string minedData = bMsg->getTransactions(k).getData();
        mempool.erase(std::remove_if(mempool.begin(), mempool.end(),
            [&](const TransactionMsg& t) { return std::string(t.getData()) == minedData; }),
            mempool.end());
    }
}

void Node::startMining() {
    if (isSeed || !pendingQueue.empty()) return; 
    if (miningTimer->isScheduled()) cancelEvent(miningTimer);
    double lambda = (hashPower * (1.0/meanBlockTime)) / 100.0;
    scheduleAt(simTime() + exponential(1.0 / lambda), miningTimer);
}

void Node::sendLivenessPing() {
    GossipMsg *ping = new GossipMsg("Liveness");
    ping->setType(3);
    std::string format = std::to_string(simTime().dbl()) + ":" + myIp + ":" + std::to_string(gossipCounter++);
    ping->setGossipFormat(format.c_str());
    broadcast(ping); 
}

void Node::checkLiveness() {
    for (int i = 0; i < gateSize("port"); i++) {
        if (!gate("port$o", i)->isConnected()) continue;
        if (simTime().dbl() - lastHeardFrom[i] > 39.0) {
            cModule* deadMod = gate("port$o", i)->getPathEndGate()->getOwnerModule();
            std::string deadIp = deadMod->hasPar("myIp") ? deadMod->par("myIp").stdstringValue() : "Unknown";

            std::string deadMsgStr = "Dead Node:" + deadIp + ":" + std::to_string(i) + ":" + std::to_string(simTime().dbl()) + ":" + myIp;
            GossipMsg *deadAlert = new GossipMsg("DeadNodeAlert");
            deadAlert->setType(4);
            deadAlert->setPayload(deadMsgStr.c_str());
            broadcast(deadAlert, true); 
            gate("port$o", i)->disconnect();
            EV << "REPORTED DEAD NODE: " << deadMsgStr << endl;
        }
    }
}
void Node::broadcast(cMessage *msg, bool toSeedsOnly) {
    for (int i = 0; i < gateSize("port"); i++) {
        if (gate("port$o", i)->isConnected()) {
            cModule* remoteMod = gate("port$o", i)->getPathEndGate()->getOwnerModule();
            bool isConnectedToSeed = (connectedSeedIds.find(remoteMod->getId()) != connectedSeedIds.end());
            
            if (toSeedsOnly && !isConnectedToSeed) continue;
            send(msg->dup(), "port$o", i);
        }
    }
    delete msg; 
}

void Node::finish() { 
    if (keyPair) EVP_PKEY_free(keyPair);
    for(auto& pair : pendingQueue) delete pair.second;
    cancelAndDelete(miningTimer);
    cancelAndDelete(livenessTimer);
    cancelAndDelete(txTimer);
    cancelAndDelete(plRetryTimer);
    cancelAndDelete(killTimer);
    cancelAndDelete(invalidTxTimer);
    cancelAndDelete(doubleSpendTimer);
    cancelAndDelete(invalidBlockTimer);
}