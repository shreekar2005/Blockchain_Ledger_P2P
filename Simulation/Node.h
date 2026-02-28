#ifndef __BLOCKCHAINSIM_NODE_H_
#define __BLOCKCHAINSIM_NODE_H_

#include <omnetpp.h>
#include <map>
#include <set>
#include <vector>
#include <cmath>
#include <string>
#include "Blockchain_m.h"
#include "CryptoUtils.h"
#include "BlockchainLedger.h"

using namespace omnetpp;

class Node : public cSimpleModule {
  private:
    BlockchainLedger ledger;                      
    EVP_PKEY *keyPair;                            
    std::string walletAddress;

    bool isSeed;                                  
    std::string myIp;                             
    double hashPower;                             
    double meanBlockTime;                         
    
    // P2P Registration Variables
    int totalSeedsInNetwork;
    int numSeedsToRegister;
    int plResponsesReceived;
    std::vector<std::string> allSeedIps;          
    std::set<int> connectedSeedIds;               
    std::set<int> knownPeerIds;             
    std::vector<int> activeConnections;   
    
    std::map<int, std::pair<std::string, int>> neighborInfo; // gateIndex -> {IP, Port}
    
    // Sync & Gossip State
    bool isSynced;
    std::map<int, BlockMsg*> pendingQueue;
    std::set<std::string> messageList;            
    int gossipCounter;                            
    std::map<int, simtime_t> lastHeardFrom;       

    // Mempool for Transactions
    std::vector<TransactionMsg> mempool;

    cMessage *miningTimer;                        
    cMessage *livenessTimer;   
    cMessage *txTimer; 
    cMessage *plRetryTimer; 
    cMessage *killTimer; 
    cMessage *invalidTxTimer;
    cMessage *doubleSpendTimer;
    cMessage *invalidBlockTimer;
    bool isDead; 
    bool triggerInvalidBlock; 

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    void startMining();
    void generateTransaction();
    void cleanMempool(BlockMsg* bMsg);
    void checkLiveness();
    void sendLivenessPing();
    
    // Dynamic Networking
    void connectToNode(cModule* targetNode);
    void initiatePeerRegistration();
    void processPeerList(PeerListMsg* msg);
    void establishTCPConnections();
    
    void handleSync(BlockMsg* bMsg);
    void processPendingQueue();
    
    void broadcast(cMessage *msg, bool toSeedsOnly = false);
};

#endif