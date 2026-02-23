// BlockchainLedger.h

#ifndef BLOCKCHAINLEDGER_H_
#define BLOCKCHAINLEDGER_H_

#include <vector>
#include <string>
#include <set>
#include "Blockchain_m.h"

/**
 * @brief internal block structure stored on node's hard drive
 */
struct LocalBlock {
    int index;                                    ///< block height
    std::string prevHash;                         ///< hash of previous block
    std::string merkleRoot;                       ///< combined hash of all txs
    std::string hash;                             ///< hash of this block
    std::vector<std::string> transactions;        ///< list of tx strings
};

/**
 * @brief manages the actual blockchain and double spending protection
 */
class BlockchainLedger {
private:
    std::vector<LocalBlock> chain;                ///< stores the local copy of blockchain
    std::set<std::string> unspentAssets;          ///< utxo set to track available assets and catch double spends

public:
    BlockchainLedger();
    
    /**
     * @brief creates the very first block at index 0
     */
    void createGenesis();
    
    /**
     * @brief packages unspent assets into a new block for mining
     * @param myIndex the numerical index of the miner node
     * @param myIp the string ip identifier of the miner node
     * @return a new localblock ready to be broadcasted
     */
    LocalBlock createCandidateBlock(int myIndex, std::string myIp);
    
    /**
     * @brief validates incoming network block and adds if it follows longest chain rule
     * @param bMsg the block message received from the network
     * @return 1 if accepted (longer chain), 0 if ignored, -1 if rejected (double spend)
     */
    int validateAndAddBlock(BlockMsg *bMsg);
    
    /**
     * @brief returns total number of blocks in current chain
     * @return integer representing chain height
     */
    int getHeight();
    
    /**
     * @brief returns the hash of the latest block in chain
     * @return string representing the latest block's hash
     */
    std::string getHeadHash();
    
    /**
     * @brief recursively hashes transaction pairs to get single root hash
     * @param txs vector of transaction strings to be hashed
     * @return the final single merkle root string
     */
    std::string calculateMerkleRoot(std::vector<std::string> txs);
};

#endif