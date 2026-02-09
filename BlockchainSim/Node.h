#ifndef __BLOCKCHAINSIM_NODE_H_
#define __BLOCKCHAINSIM_NODE_H_

#include <omnetpp.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "Blockchain_m.h"

using namespace omnetpp;

class Node : public cSimpleModule
{
  private:
    struct Block {
        int index;
        std::string prevHash;
        std::string hash;
        std::string data;
        std::vector<std::string> transactions;
    };

    // Identity
    bool isSeed;
    bool isOnline; // NEW: Status flag
    std::string myIp;
    EC_KEY *keyPair;
    std::string walletAddress;

    // State
    std::vector<Block> chain;
    std::set<std::string> messageList;
    int gossipCounter;

    // Liveness Tracking
    std::map<int, simtime_t> lastHeardFrom;

    // Mining & Lifecycle
    double hashPower;
    cMessage *miningTimer;
    cMessage *livenessTimer;
    cMessage *startUpTimer;   // NEW
    cMessage *shutdownTimer;  // NEW
    double meanBlockTime;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    // Logic Functions
    void generateIdentity();
    void startMining();
    void handleGossip(GossipMsg *msg);
    void broadcast(cMessage *msg, bool excludeSender = false, int senderGate = -1);
    void checkLiveness();
    void reportDeadNode(int gateIndex);
    std::string calculateHash(int index, std::string prev, std::string data);
    bool processBlock(BlockMsg *bMsg);
};

#endif
