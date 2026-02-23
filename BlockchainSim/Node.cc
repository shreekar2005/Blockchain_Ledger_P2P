// Node.cc

#include "Node.h"

Define_Module(Node);

void Node::initialize()
{
    isSeed = par("isSeed");
    hashPower = par("hashPower");
    myIp = par("myIp").stdstringValue() + "_" + std::to_string(getIndex());
    double startUpTime = par("startUpTime");
    double shutdownTime = par("shutdownTime");
    
    isOnline = false;
    gossipCounter = 0;
    meanBlockTime = 2.0;

    keyPair = CryptoUtils::generateKey();
    ledger.createGenesis();

    miningTimer = new cMessage("MiningTimer");
    livenessTimer = new cMessage("LivenessTimer");
    startUpTimer = new cMessage("StartUp");
    shutdownTimer = new cMessage("Shutdown");
    doubleSpendTimer = new cMessage("DoubleSpendAttack");

    scheduleAt(startUpTime, startUpTimer);
    if (shutdownTime >= 0) scheduleAt(shutdownTime, shutdownTimer);
    
    if (!isSeed && getIndex() == 2) scheduleAt(40.0, doubleSpendTimer);
}

void Node::handleMessage(cMessage *msg)
{
    if (msg == startUpTimer) {
        isOnline = true;
        EV << (isSeed ? "SEED" : "PEER") << " " << myIp << " ONLINE." << endl;
        if (!isSeed) startMining();
        scheduleAt(simTime() + 1.0, livenessTimer); 
        return; 
    }
    if (msg == shutdownTimer) {
        isOnline = false;
        EV << "NODE " << myIp << " CRASHED." << endl;
        if (miningTimer->isScheduled()) cancelEvent(miningTimer);
        return;
    }
    if (msg == doubleSpendTimer) {
        performDoubleSpendAttack();
        return;
    }

    if (!isOnline) { if (!msg->isSelfMessage()) delete msg; return; }

    if (msg == miningTimer) {
        LocalBlock b = ledger.createCandidateBlock(getIndex(), myIp);
        
        BlockMsg *bMsg = new BlockMsg("BlockData");
        bMsg->setIndex(b.index);
        bMsg->setCurrentHash(b.hash.c_str());
        bMsg->setPreviousHash(b.prevHash.c_str());
        bMsg->setMerkleRoot(b.merkleRoot.c_str());
        bMsg->setTransactionsArraySize(b.transactions.size());
        for(size_t k=0; k<b.transactions.size(); k++) {
             bMsg->setTransactions(k, b.transactions[k].c_str());
        }

        ledger.validateAndAddBlock(bMsg); 
        
        EV << "MINED BLOCK " << b.index << " | Root: " << b.merkleRoot.substr(0,8) << endl;
        broadcast(bMsg->dup());
        delete bMsg;
        
        startMining();
    }
    else if (msg == livenessTimer) {
        checkLiveness();
        scheduleAt(simTime() + 13.0, livenessTimer);
    }
    else {
        int gateIndex = msg->getArrivalGate()->getIndex();
        lastHeardFrom[gateIndex] = simTime();

        BlockMsg *bMsg = dynamic_cast<BlockMsg*>(msg);
        if (bMsg) {
            int result = ledger.validateAndAddBlock(bMsg);
            if (result == 1) {
                EV << "Received longer chain (Block " << bMsg->getIndex() << "). Switching..." << endl;
                if (miningTimer->isScheduled()) cancelEvent(miningTimer);
                startMining();
                broadcast(bMsg->dup(), true, gateIndex);
            }
            else if (result == -1) {
                EV << "SECURITY ALERT: Double Spending in Block " << bMsg->getIndex() << "!" << endl;
            }
        }
        
        GossipMsg *gMsg = dynamic_cast<GossipMsg*>(msg);
        if (gMsg) handleGossip(gMsg);
        
        delete msg; 
    }
}

void Node::performDoubleSpendAttack() {
    EV << "!!! INITIATING DOUBLE SPEND ATTACK !!!" << endl;
    BlockMsg *maliciousMsg = new BlockMsg("FakeBlock");
    maliciousMsg->setIndex(ledger.getHeight());
    maliciousMsg->setCurrentHash("FAKE_HASH");
    maliciousMsg->setTransactionsArraySize(2);

    maliciousMsg->setTransactions(0, "SPEND:Genesis_Asset");
    maliciousMsg->setTransactions(1, "CREATE:Hacker_Asset");
    
    broadcast(maliciousMsg);
}

void Node::checkLiveness() {
    GossipMsg *ping = new GossipMsg("LivenessPing");
    ping->setType(3);
    ping->setOriginatorIp(myIp.c_str());
    ping->setMsgId(gossipCounter++);
    broadcast(ping);

    for (int i = 0; i < gateSize("port"); i++) {
        if (!gate("port$o", i)->isConnected()) continue;
        if (lastHeardFrom.find(i) != lastHeardFrom.end()) {
            if (simTime() - lastHeardFrom[i] > 39.0) {
                 EV << "WARNING: Neighbor on gate " << i << " is DEAD!" << endl;
                 lastHeardFrom[i] = simTime(); 
            }
        } else { lastHeardFrom[i] = simTime(); }
    }
}

void Node::handleGossip(GossipMsg *msg) {
    std::string uniqueId = std::string(msg->getOriginatorIp()) + "_" + std::to_string(msg->getMsgId());
    if (messageList.find(uniqueId) != messageList.end()) return; 
    messageList.insert(uniqueId);
    if (msg->getType() == 4) broadcast(msg->dup(), true, msg->getArrivalGate()->getIndex());
}

void Node::startMining() {
    if (isSeed) return; 
    if (miningTimer->isScheduled()) cancelEvent(miningTimer);
    double lambda = (hashPower * (1.0/meanBlockTime)) / 100.0;
    if (lambda <= 0) return;
    scheduleAt(simTime() + exponential(1.0 / lambda), miningTimer);
}

void Node::broadcast(cMessage *msg, bool excludeSender, int senderGate) {
    int n = gateSize("port");
    for (int i = 0; i < n; i++) {
        if (gate("port$o", i)->isConnected()) {
            if (excludeSender && i == senderGate) continue;
            send(msg->dup(), "port$o", i);
        }
    }
    if (!excludeSender) delete msg; else delete msg; 
}

void Node::finish() { 
    if (keyPair) EVP_PKEY_free(keyPair);
}