/**
 * @file Node.h
 * @brief Defines the Node class which represents a peer in the P2P blockchain network.
 */

#ifndef BLOCKCHAIN_SIM_NODE_H_
#define BLOCKCHAIN_SIM_NODE_H_

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
using namespace std;

/**
 * @class Node
 * @brief Represents a participant in the blockchain network, handling mining, transactions, and P2P communication.
 */
class Node : public cSimpleModule {
  private:
    BlockchainLedger ledger;                      ///< Local copy of the blockchain ledger
    EVP_PKEY *keyPair;                            ///< ECDSA key pair for the node
    string walletAddress;                    ///< Derived wallet address for this node

    bool isSeed;                                  ///< Flag indicating if this node is a seed node
    string myIp;                             ///< IP address of the node
    double hashPower;                             ///< Mining power of the node
    double meanBlockTime;                         ///< Average time to mine a block
    
    // P2P Registration Variables
    int totalSeedsInNetwork;                      ///< Total number of seed nodes in the network
    int numSeedsToRegister;                       ///< Number of seeds to contact for registration
    int plResponsesReceived;                      ///< Counter for Peer List responses
    vector<string> allSeedIps;          ///< List of all known seed IP addresses
    set<int> connectedSeedIds;               ///< Set of IDs of connected seed nodes
    set<int> knownPeerIds;                   ///< Set of IDs of known peer nodes
    vector<int> activeConnections;           ///< List of active outgoing connections
    
    map<int, pair<string, int>> neighborInfo; ///< Map of gate index to neighbor info {IP, Port}
    
    // Sync & Gossip State
    bool isSynced;                                ///< Flag indicating if the node is synced with the network
    map<int, BlockMsg*> pendingQueue;        ///< Queue for blocks received out of order
    set<string> messageList;            ///< Set of processed message IDs to prevent duplicates
    int gossipCounter;                            ///< Counter for gossip message tracking
    map<int, simtime_t> lastHeardFrom;       ///< Last time a message was received from a neighbor

    // Mempool for Transactions
    vector<TransactionMsg> mempool;          ///< Collection of pending transactions

    cMessage *miningTimer;                        ///< Timer for the mining process
    cMessage *livenessTimer;                      ///< Timer for periodic liveness checks
    cMessage *txTimer;                            ///< Timer for generating new transactions
    cMessage *plRetryTimer;                       ///< Timer for retrying peer list registration
    cMessage *killTimer;                          ///< Timer to simulate node failure
    cMessage *invalidTxTimer;                     ///< Timer for generating invalid transactions
    cMessage *doubleSpendTimer;                   ///< Timer for generating double spend attempts
    cMessage *invalidBlockTimer;                  ///< Timer for generating invalid blocks
    
    bool isDead;                                  ///< Flag indicating if the node has failed
    bool triggerInvalidBlock;                     ///< Flag to trigger the creation of an invalid block

  protected:
    /**
     * @brief OMNeT++ initialization function.
     */
    virtual void initialize() override;

    /**
     * @brief OMNeT++ message handling function.
     * @param msg The received message.
     */
    virtual void handleMessage(cMessage *msg) override;

    /**
     * @brief OMNeT++ finish function called at simulation end.
     */
    virtual void finish() override;

    /**
     * @brief Starts the mining process by scheduling the mining timer.
     */
    void startMining();

    /**
     * @brief Generates and broadcasts a new transaction.
     */
    void generateTransaction();

    /**
     * @brief Removes transactions from the mempool that are included in the given block.
     * @param bMsg The block message containing confirmed transactions.
     */
    void cleanMempool(BlockMsg* bMsg);

    /**
     * @brief Checks the liveness of neighbors and manages connection timeouts.
     */
    void checkLiveness();

    /**
     * @brief Sends a liveness ping to all neighbors.
     */
    void sendLivenessPing();
    
    /**
     * @brief Establishes a connection to a target node.
     * @param targetNode Pointer to the target OMNeT++ module.
     */
    void connectToNode(cModule* targetNode);

    /**
     * @brief Initiates the peer registration process with seed nodes.
     */
    void initiatePeerRegistration();

    /**
     * @brief Processes a received PeerListMsg.
     * @param msg The received peer list message.
     */
    void processPeerList(PeerListMsg* msg);

    /**
     * @brief Establishes TCP-like connections based on the peer list.
     */
    void establishTCPConnections();
    
    /**
     * @brief Handles blockchain synchronization with received blocks.
     * @param bMsg The received block message.
     */
    void handleSync(BlockMsg* bMsg);

    /**
     * @brief Processes the queue of pending blocks.
     */
    void processPendingQueue();
    
    /**
     * @brief Broadcasts a message to all connected neighbors.
     * @param msg The message to broadcast.
     * @param toSeedsOnly If true, only broadcast to seed nodes.
     */
    void broadcast(cMessage *msg, bool toSeedsOnly = false);
};

#endif // BLOCKCHAIN_SIM_NODE_H_
