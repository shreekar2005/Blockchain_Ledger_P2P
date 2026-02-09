#include "BlockchainLedger.h"
#include "CryptoUtils.h"
#include <iostream>

BlockchainLedger::BlockchainLedger() {}

void BlockchainLedger::createGenesis() {
    LocalBlock genesis;
    genesis.index = 0;
    genesis.prevHash = "0000";
    genesis.transactions.push_back("Genesis_Tx");
    genesis.merkleRoot = calculateMerkleRoot(genesis.transactions);
    genesis.hash = CryptoUtils::sha256(std::to_string(genesis.index) + genesis.prevHash + genesis.merkleRoot);
    chain.push_back(genesis);
}

int BlockchainLedger::getHeight() {
    return chain.size();
}

std::string BlockchainLedger::getHeadHash() {
    return chain.back().hash;
}

LocalBlock BlockchainLedger::createCandidateBlock(int myIndex, std::string myIp) {
    LocalBlock newBlock;
    newBlock.index = chain.size();
    newBlock.prevHash = chain.back().hash;
    
    // Create a transaction (Asset ID based on time + node ID)
    // In a real app, this would come from a mempool.
    std::string txId = "Oil_" + std::to_string(chain.size()) + "_" + std::to_string(myIndex);
    newBlock.transactions.push_back(txId);
    
    newBlock.merkleRoot = calculateMerkleRoot(newBlock.transactions);
    newBlock.hash = CryptoUtils::sha256(std::to_string(newBlock.index) + newBlock.prevHash + newBlock.merkleRoot);
    
    return newBlock;
}

int BlockchainLedger::validateAndAddBlock(BlockMsg *bMsg) {
    // 1. Check for Duplicate
    for(auto &b : chain) {
        if (b.hash == bMsg->getCurrentHash()) return 0; // Already have it
    }

    // 2. Check for Double Spending
    int n = bMsg->getTransactionsArraySize();
    for(int k=0; k<n; k++) {
        std::string tx = bMsg->getTransactions(k);
        if (spentAssets.find(tx) != spentAssets.end()) {
            return -1; // REJECT: Double Spend Detected
        }
    }

    // 3. Longest Chain Rule
    if (bMsg->getIndex() > (int)chain.size() - 1) {
        // ACCEPT
        LocalBlock newB;
        newB.index = bMsg->getIndex();
        newB.hash = bMsg->getCurrentHash();
        newB.prevHash = bMsg->getPreviousHash();
        newB.merkleRoot = bMsg->getMerkleRoot();
        
        for(int k=0; k<n; k++) {
            newB.transactions.push_back(bMsg->getTransactions(k));
            spentAssets.insert(bMsg->getTransactions(k)); // Update Ledger
        }
        
        chain.push_back(newB);
        return 1; // Success: New Longest Chain
    }

    return 0; // Valid block but not longer than what we have
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