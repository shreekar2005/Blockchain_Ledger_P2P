#include "Node.h"

Define_Module(Node);

void Node::initialize()
{
    // 1. Config
    isSeed = par("isSeed");
    hashPower = par("hashPower");
    myIp = par("myIp").stdstringValue() + "_" + std::to_string(getIndex());
    double startUpTime = par("startUpTime");
    double shutdownTime = par("shutdownTime");
    
    isOnline = false;
    gossipCounter = 0;
    meanBlockTime = 2.0;

    // 2. Setup Modules
    keyPair = CryptoUtils::generateKey();
    ledger.createGenesis(); // Initialize Ledger

    // 3. Timers
    miningTimer = new cMessage("MiningTimer");
    livenessTimer = new cMessage("LivenessTimer");
    startUpTimer = new cMessage("StartUp");
    shutdownTimer = new cMessage("Shutdown");
    doubleSpendTimer = new cMessage("DoubleSpendAttack");

    // 4. Schedule Lifecycle
    scheduleAt(startUpTime, startUpTimer);
    if (shutdownTime >= 0) scheduleAt(shutdownTime, shutdownTimer);
    
    // Schedule Attack for Node 2 at 40s
    if (!isSeed && getIndex() == 2) scheduleAt(40.0, doubleSpendTimer);
}

void Node::handleMessage(cMessage *msg)
{
    // --- LIFECYCLE ---
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

    // --- MINING ---
    if (msg == miningTimer) {
        // Delegate logic to Ledger Module
        LocalBlock b = ledger.createCandidateBlock(getIndex(), myIp);
        
        // Convert to Message
        BlockMsg *bMsg = new BlockMsg("BlockData");
        bMsg->setIndex(b.index);
        bMsg->setCurrentHash(b.hash.c_str());
        bMsg->setPreviousHash(b.prevHash.c_str());
        bMsg->setMerkleRoot(b.merkleRoot.c_str());
        bMsg->setTransactionsArraySize(b.transactions.size());
        for(size_t k=0; k<b.transactions.size(); k++) {
             bMsg->setTransactions(k, b.transactions[k].c_str());
        }

        // Add to own ledger first
        ledger.validateAndAddBlock(bMsg); 
        
        EV << "MINED BLOCK " << b.index << " | Root: " << b.merkleRoot.substr(0,8) << endl;
        broadcast(bMsg->dup()); // Broadcast copy
        delete bMsg; // Delete original
        
        startMining();
    }
    // --- LIVENESS ---
    else if (msg == livenessTimer) {
        checkLiveness();
        scheduleAt(simTime() + 13.0, livenessTimer);
    }
    // --- NETWORK ---
    else {
        int gateIndex = msg->getArrivalGate()->getIndex();
        lastHeardFrom[gateIndex] = simTime();

        BlockMsg *bMsg = dynamic_cast<BlockMsg*>(msg);
        if (bMsg) {
            // Delegate Validation to Ledger Module
            int result = ledger.validateAndAddBlock(bMsg);
            
            if (result == 1) {
                // Accepted & Longer -> Switch & Broadcast
                EV << "Received longer chain (Block " << bMsg->getIndex() << "). Switching..." << endl;
                if (miningTimer->isScheduled()) cancelEvent(miningTimer);
                startMining();
                broadcast(bMsg->dup(), true, gateIndex);
            }
            else if (result == -1) {
                // Double Spend Detected
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
    // Maliciously craft a block with an old asset
    BlockMsg *maliciousMsg = new BlockMsg("FakeBlock");
    maliciousMsg->setIndex(ledger.getHeight()); // Try to extend current
    maliciousMsg->setCurrentHash("FAKE_HASH");
    maliciousMsg->setTransactionsArraySize(1);
    maliciousMsg->setTransactions(0, "Oil_0_0"); // Assume Genesis asset "Oil_0_0" is already spent
    
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
                 // Add report logic here if needed
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
    if (keyPair) EC_KEY_free(keyPair); 
}