#ifndef BLOCKCHAINLEDGER_H_
#define BLOCKCHAINLEDGER_H_

#include <vector>
#include <string>
#include <set>
#include <openssl/evp.h> 
#include "Blockchain_m.h"

struct LocalBlock {
    int index;                                    
    std::string prevHash;                         
    std::string merkleRoot;                       
    std::string hash;                             
    std::vector<TransactionMsg> transactions;        
};

class BlockchainLedger {
private:
    std::vector<LocalBlock> chain;                
    std::set<std::string> unspentAssets;          

public:
    BlockchainLedger();
    void createGenesis();
    // Now accepts the mempool to pack unconfirmed TXs into the block
    LocalBlock createCandidateBlock(int myIndex, std::string myIp, EVP_PKEY* myKey, std::string myAddr, std::vector<TransactionMsg>& mempool);
    int validateAndAddBlock(BlockMsg *bMsg, double currentSimTime);
    int getHeight();
    std::string getHeadHash();
    std::string calculateMerkleRoot(std::vector<TransactionMsg>& txs);
    bool validateTransaction(const TransactionMsg& tx);
};

#endif