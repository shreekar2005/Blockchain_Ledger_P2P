// BlockchainLedger.cc

#include "BlockchainLedger.h"
#include "CryptoUtils.h"
#include <iostream>

BlockchainLedger::BlockchainLedger() {}

void BlockchainLedger::createGenesis() {
    LocalBlock genesis;
    genesis.index = 0;
    genesis.prevHash = "0000";

    genesis.transactions.push_back("CREATE:Genesis_Asset");

    genesis.merkleRoot = calculateMerkleRoot(genesis.transactions);
    genesis.hash = CryptoUtils::sha256(std::to_string(genesis.index) + genesis.prevHash + genesis.merkleRoot);
    chain.push_back(genesis);

    unspentAssets.insert("Genesis_Asset");
}

int BlockchainLedger::getHeight() { return chain.size(); }
std::string BlockchainLedger::getHeadHash() { return chain.back().hash; }

LocalBlock BlockchainLedger::createCandidateBlock(int myIndex, std::string myIp) {
    LocalBlock newBlock;
    newBlock.index = chain.size();
    newBlock.prevHash = chain.back().hash;
    
    if (!unspentAssets.empty()) {
        std::string assetToSpend = *unspentAssets.begin();
        newBlock.transactions.push_back("SPEND:" + assetToSpend);
    }

    std::string asset1 = "Oil_BatchA_" + std::to_string(chain.size()) + "_" + std::to_string(myIndex);
    std::string asset2 = "Oil_BatchB_" + std::to_string(chain.size()) + "_" + std::to_string(myIndex);
    std::string asset3 = "Oil_BatchC_" + std::to_string(chain.size()) + "_" + std::to_string(myIndex);

    newBlock.transactions.push_back("CREATE:" + asset1);
    newBlock.transactions.push_back("CREATE:" + asset2);
    newBlock.transactions.push_back("CREATE:" + asset3);
    
    newBlock.merkleRoot = calculateMerkleRoot(newBlock.transactions);
    newBlock.hash = CryptoUtils::sha256(std::to_string(newBlock.index) + newBlock.prevHash + newBlock.merkleRoot);
    
    return newBlock;
}

int BlockchainLedger::validateAndAddBlock(BlockMsg *bMsg) {
    for(auto &b : chain) {
        if (b.hash == bMsg->getCurrentHash()) return 0; 
    }

    std::vector<std::string> spentInThisBlock;
    std::vector<std::string> createdInThisBlock;

    int n = bMsg->getTransactionsArraySize();
    for(int k=0; k<n; k++) {
        std::string tx = bMsg->getTransactions(k);

        if (tx.find("SPEND:") == 0) {
            std::string asset = tx.substr(6);
            if (unspentAssets.find(asset) == unspentAssets.end()) {
                return -1; 
            }
            spentInThisBlock.push_back(asset);
        }
        else if (tx.find("CREATE:") == 0) {
            std::string asset = tx.substr(7);
            createdInThisBlock.push_back(asset);
        }
    }

    if (bMsg->getIndex() > (int)chain.size() - 1) {
        LocalBlock newB;
        newB.index = bMsg->getIndex();
        newB.hash = bMsg->getCurrentHash();
        newB.prevHash = bMsg->getPreviousHash();
        newB.merkleRoot = bMsg->getMerkleRoot();
        
        for(int k=0; k<n; k++) newB.transactions.push_back(bMsg->getTransactions(k));

        for (const std::string& spent : spentInThisBlock) unspentAssets.erase(spent);
        for (const std::string& created : createdInThisBlock) unspentAssets.insert(created);
        
        chain.push_back(newB);
        return 1; 
    }
    return 0;
}

std::string BlockchainLedger::calculateMerkleRoot(std::vector<std::string> txs) {
    if (txs.empty()) return "";
    if (txs.size() == 1) return CryptoUtils::sha256(txs[0]);
    
    std::vector<std::string> currentLevel = txs;
    while (currentLevel.size() > 1) {
        std::vector<std::string> nextLevel;
        for (size_t i = 0; i < currentLevel.size(); i += 2) {
            if (i + 1 < currentLevel.size()) {
                nextLevel.push_back(CryptoUtils::sha256(currentLevel[i] + currentLevel[i+1]));
            } else {
                nextLevel.push_back(CryptoUtils::sha256(currentLevel[i] + currentLevel[i]));
            }
        }
        currentLevel = nextLevel;
    }
    return currentLevel[0];
}