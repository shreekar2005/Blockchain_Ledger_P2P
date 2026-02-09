#include "Node.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

Define_Module(Node);

// Helper for SHA256 hashing
std::string sha256(const std::string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

void Node::initialize()
{
    // 1. Load Parameters
    isSeed = par("isSeed");
    hashPower = par("hashPower");
    myIp = par("myIp").stdstringValue() + "_" + std::to_string(getIndex());

    // 2. Lifecycle Parameters
    double startUpTime = par("startUpTime");
    double shutdownTime = par("shutdownTime");

    isOnline = false; // Default off, will turn on at startUpTime
    gossipCounter = 0;

    // 3. Crypto Setup
    generateIdentity();

    // 4. Timers
    miningTimer = new cMessage("MiningTimer");
    livenessTimer = new cMessage("LivenessTimer");
    startUpTimer = new cMessage("StartUp");
    shutdownTimer = new cMessage("Shutdown");

    meanBlockTime = 2.0;

    // Genesis Block
    Block genesis;
    genesis.index = 0;
    genesis.prevHash = "0000";
    genesis.hash = sha256("genesis");
    chain.push_back(genesis);

    // Schedule Lifecycle Events
    scheduleAt(startUpTime, startUpTimer);

    if (shutdownTime >= 0) {
        scheduleAt(shutdownTime, shutdownTimer);
    }
}

void Node::handleMessage(cMessage *msg)
{
    // --- LIFECYCLE MANAGEMENT ---

    if (msg == startUpTimer) {
        isOnline = true;
        if (isSeed) {
            EV << "SEED NODE " << myIp << " CAME ONLINE." << endl;
        } else {
            EV << "PEER NODE " << myIp << " CAME ONLINE. SYNCING..." << endl;
            startMining();
        }
        // Start Heartbeat
        scheduleAt(simTime() + 1.0, livenessTimer);
        return;
    }

    if (msg == shutdownTimer) {
        isOnline = false;
        EV << "NODE " << myIp << " CRASHED / DIED (Simulated Failure)." << endl;
        if (miningTimer->isScheduled()) cancelEvent(miningTimer);
        if (livenessTimer->isScheduled()) cancelEvent(livenessTimer);
        return;
    }

    // --- IF OFFLINE, DO NOTHING ---
    if (!isOnline) {
        // If we are dead, we ignore all incoming messages (packet loss)
        if (!msg->isSelfMessage()) delete msg;
        return;
    }

    // --- NORMAL OPERATION ---

    if (msg == miningTimer) {
        Block newBlock;
        newBlock.index = chain.size();
        newBlock.prevHash = chain.back().hash;
        newBlock.data = "Block_" + std::to_string(newBlock.index);
        newBlock.hash = calculateHash(newBlock.index, newBlock.prevHash, newBlock.data);
        newBlock.transactions.push_back("Coinbase_Tx");

        chain.push_back(newBlock);
        EV << "MINED BLOCK " << newBlock.index << "! Hash: " << newBlock.hash << endl;

        BlockMsg *bMsg = new BlockMsg("BlockData");
        bMsg->setIndex(newBlock.index);
        bMsg->setCurrentHash(newBlock.hash.c_str());
        bMsg->setPreviousHash(newBlock.prevHash.c_str());

        bMsg->setTransactionsArraySize(newBlock.transactions.size());
        for(size_t k=0; k<newBlock.transactions.size(); k++){
             bMsg->setTransactions(k, newBlock.transactions[k].c_str());
        }

        broadcast(bMsg);
        startMining();
    }
    else if (msg == livenessTimer) {
        checkLiveness();
        scheduleAt(simTime() + 13.0, livenessTimer);
    }
    else {
        // Network Message
        int gateIndex = msg->getArrivalGate()->getIndex();
        lastHeardFrom[gateIndex] = simTime();

        BlockMsg *bMsg = dynamic_cast<BlockMsg*>(msg);
        if (bMsg) {
            bool isNew = processBlock(bMsg);
            if (isNew) broadcast(bMsg->dup(), true, gateIndex);
        }

        GossipMsg *gMsg = dynamic_cast<GossipMsg*>(msg);
        if (gMsg) {
            handleGossip(gMsg);
        }

        delete msg;
    }
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
                reportDeadNode(i);
                // Reset to avoid spamming the same report
                lastHeardFrom[i] = simTime();
            }
        } else {
            lastHeardFrom[i] = simTime();
        }
    }
}

void Node::reportDeadNode(int gateIndex) {
    GossipMsg *report = new GossipMsg("DeadReport");
    report->setType(4);
    report->setOriginatorIp(myIp.c_str());
    report->setMsgId(gossipCounter++);

    std::string payloadStr = "DeadNode_Detected_on_Gate_" + std::to_string(gateIndex);
    report->setPayload(payloadStr.c_str());

    broadcast(report);
}

void Node::handleGossip(GossipMsg *msg) {
    std::string uniqueId = std::string(msg->getOriginatorIp()) + "_" + std::to_string(msg->getMsgId());

    if (messageList.find(uniqueId) != messageList.end()) return;
    messageList.insert(uniqueId);

    if (msg->getType() == 4) {
        EV << "Received Dead Node Report: " << msg->getPayload() << endl;
        broadcast(msg->dup(), true, msg->getArrivalGate()->getIndex());
    }
}

void Node::startMining() {
    if (isSeed) return;

    if (miningTimer->isScheduled()) cancelEvent(miningTimer);

    double lambda = (hashPower * (1.0/meanBlockTime)) / 100.0;
    if (lambda <= 0) return;

    double waitTime = exponential(1.0 / lambda);
    scheduleAt(simTime() + waitTime, miningTimer);
}

bool Node::processBlock(BlockMsg *bMsg) {
    for(auto &b : chain) {
        if (b.hash == bMsg->getCurrentHash()) return false;
    }

    if (bMsg->getIndex() > (int)chain.size() - 1) {
        EV << "Received longer chain (Block " << bMsg->getIndex() << "). Switching..." << endl;
        if (miningTimer->isScheduled()) cancelEvent(miningTimer);

        Block newB;
        newB.index = bMsg->getIndex();
        newB.hash = bMsg->getCurrentHash();
        newB.prevHash = bMsg->getPreviousHash();

        int n = bMsg->getTransactionsArraySize();
        for(int k=0; k<n; k++) newB.transactions.push_back(bMsg->getTransactions(k));

        chain.push_back(newB);
        startMining();
        return true;
    }
    return false;
}

void Node::broadcast(cMessage *msg, bool excludeSender, int senderGate) {
    int n = gateSize("port");
    for (int i = 0; i < n; i++) {
        if (gate("port$o", i)->isConnected()) {
            if (excludeSender && i == senderGate) continue;
            send(msg->dup(), "port$o", i);
        }
    }
    if (!excludeSender) delete msg;
    else delete msg;
}

void Node::generateIdentity() {
    keyPair = EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_KEY_generate_key(keyPair);
    walletAddress = "0x" + std::to_string(getIndex()) + "ABC";
}

std::string Node::calculateHash(int index, std::string prev, std::string data) {
    return sha256(std::to_string(index) + prev + data);
}

void Node::finish() {
    if (keyPair) EC_KEY_free(keyPair);
}
