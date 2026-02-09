#ifndef __BLOCKCHAINSIM_NODE_H_
#define __BLOCKCHAINSIM_NODE_H_

#include <omnetpp.h>
#include <openssl/ec.h> // Still needed for Identity key storage
#include "Blockchain_m.h"
#include "CryptoUtils.h"
#include "BlockchainLedger.h" // Import our new module

using namespace omnetpp;

class Node : public cSimpleModule
{
  private:
    // Modules
    BlockchainLedger ledger; 
    EC_KEY *keyPair;

    // Simulation Config
    bool isSeed;
    bool isOnline;
    std::string myIp;
    double hashPower;
    double meanBlockTime; 
    
    // Network State
    std::set<std::string> messageList; 
    int gossipCounter;
    std::map<int, simtime_t> lastHeardFrom; 

    // Timers
    cMessage *miningTimer;
    cMessage *livenessTimer;
    cMessage *startUpTimer;
    cMessage *shutdownTimer;
    cMessage *doubleSpendTimer;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    // Simulation Events
    void startMining();
    void checkLiveness();
    void performDoubleSpendAttack(); // The malicious task

    // Network Helpers
    void broadcast(cMessage *msg, bool excludeSender = false, int senderGate = -1);
    void handleGossip(GossipMsg *msg);
};

#endif