#ifndef BLOCKCHAINLEDGER_H_
#define BLOCKCHAINLEDGER_H_

#include <vector>
#include <string>
#include <set>
#include "Blockchain_m.h" // Needed for BlockMsg definition

// Internal Block Structure
struct LocalBlock {
    int index;
    std::string prevHash;
    std::string merkleRoot;
    std::string hash;
    std::vector<std::string> transactions;
};

class BlockchainLedger {
private:
    std::vector<LocalBlock> chain;
    std::set<std::string> spentAssets; // Double Spending Database

public:
    BlockchainLedger();
    
    // Core Functions
    void createGenesis();
    LocalBlock createCandidateBlock(int myIndex, std::string myIp);
    
    // Validation
    // Returns 0=Ignore, 1=Accepted(Longer Chain), -1=Rejected(Double Spend)
    int validateAndAddBlock(BlockMsg *bMsg);
    
    // Helpers
    int getHeight();
    std::string getHeadHash();
    std::string calculateMerkleRoot(std::vector<std::string> txs);
};

#endif