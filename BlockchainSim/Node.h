// Node.h

#ifndef __BLOCKCHAINSIM_NODE_H_
#define __BLOCKCHAINSIM_NODE_H_

#include <omnetpp.h>
#include <openssl/evp.h>
#include "Blockchain_m.h"
#include "CryptoUtils.h"
#include "BlockchainLedger.h"

using namespace omnetpp;

/**
 * @brief handles omnet++ simulation events for a single network node
 */
class Node : public cSimpleModule
{
  private:
    BlockchainLedger ledger;                      ///< handles block verification and storage
    EVP_PKEY *keyPair;                            ///< cryptographic keys for this node

    bool isSeed;                                  ///< true if node is a seed server
    bool isOnline;                                ///< true if node is actively running
    std::string myIp;                             ///< unique identifier for this node
    double hashPower;                             ///< computational power used for calculating block time
    double meanBlockTime;                         ///< target block time for the network
    
    std::set<std::string> messageList;            ///< keeps track of seen gossip ids to avoid infinite loops
    int gossipCounter;                            ///< counter to generate unique msg ids
    std::map<int, simtime_t> lastHeardFrom;       ///< tracks time of last ping from each neighbor gate

    cMessage *miningTimer;                        ///< self message to trigger block discovery
    cMessage *livenessTimer;                      ///< self message to trigger neighbor pings
    cMessage *startUpTimer;                       ///< self message to bring node online
    cMessage *shutdownTimer;                      ///< self message to crash the node
    cMessage *doubleSpendTimer;                   ///< self message to trigger malicious attack

  protected:
    /**
     * @brief called once at beginning of simulation to setup parameters
     */
    virtual void initialize() override;
    
    /**
     * @brief core event loop that handles timers and incoming network packets
     * @param msg the message or timer event received by the node
     */
    virtual void handleMessage(cMessage *msg) override;
    
    /**
     * @brief cleans up memory when simulation ends
     */
    virtual void finish() override;

    /**
     * @brief calculates math for pow and sets mining timer
     */
    void startMining();
    
    /**
     * @brief pings neighbors and checks if anyone missed 3 pings
     */
    void checkLiveness();
    
    /**
     * @brief malicious function that broadcasts a block with an already spent asset
     */
    void performDoubleSpendAttack();
    
    /**
     * @brief sends message out to connected neighbor gates
     * @param msg the message to broadcast
     * @param excludeSender if true, prevents sending back to the node that sent it
     * @param senderGate the gate index to exclude if excludeSender is true
     */
    void broadcast(cMessage *msg, bool excludeSender = false, int senderGate = -1);
    
    /**
     * @brief handles generic network gossip and forwards it to neighbors
     * @param msg the gossip message containing liveness or dead node reports
     */
    void handleGossip(GossipMsg *msg);
};

#endif