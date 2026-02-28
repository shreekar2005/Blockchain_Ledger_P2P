#include "BlockchainLedger.h"
#include "CryptoUtils.h"
#include <iostream>
#include <cmath>

BlockchainLedger::BlockchainLedger() {}

void BlockchainLedger::createGenesis() {
    LocalBlock genesis;
    genesis.index = 0;
    genesis.prevHash = "0000";
    
    TransactionMsg tx;
    tx.setData("CREATE:Genesis_Asset");
    genesis.transactions.push_back(tx);

    genesis.merkleRoot = calculateMerkleRoot(genesis.transactions);
    genesis.hash = CryptoUtils::sha256(std::to_string(genesis.index) + genesis.prevHash + genesis.merkleRoot);
    chain.push_back(genesis);
    unspentAssets.insert("Genesis_Asset");
}

int BlockchainLedger::getHeight() { return chain.size(); }
std::string BlockchainLedger::getHeadHash() { return chain.back().hash; }

bool BlockchainLedger::validateTransaction(const TransactionMsg& tx) {
    if (!CryptoUtils::verifySignature(tx.getData(), tx.getSenderPublicKey(), tx.getSigR(), tx.getSigS())) {
        return false;
    }
    std::string data = tx.getData();
    if (data.find("SPEND:") == 0) {
        std::string asset = data.substr(6);
        if (unspentAssets.find(asset) == unspentAssets.end()) return false;
    }
    return true;
}

int BlockchainLedger::validateAndAddBlock(BlockMsg *bMsg, double currentSimTime) {
    if (std::abs(currentSimTime - bMsg->getTimestamp()) > 3600.0) return -1;

    for(auto &b : chain) {
        if (b.hash == bMsg->getCurrentHash()) return 0; 
    }

    std::vector<std::string> spentInThisBlock;
    std::vector<std::string> createdInThisBlock;

    int n = bMsg->getTransactionsArraySize();
    for(int k=0; k<n; k++) {
        TransactionMsg tx = bMsg->getTransactions(k);
        if (!validateTransaction(tx)) return -1; 

        std::string data = tx.getData();
        if (data.find("SPEND:") == 0) spentInThisBlock.push_back(data.substr(6));
        else if (data.find("CREATE:") == 0) createdInThisBlock.push_back(data.substr(7));
    }

    if (bMsg->getIndex() > (int)chain.size() - 1) {
        // Enforce PreviousHash rule
        if (bMsg->getPreviousHash() != chain.back().hash) {
            return -1; 
        }

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

LocalBlock BlockchainLedger::createCandidateBlock(int myIndex, std::string myIp, EVP_PKEY* myKey, std::string myAddr, std::vector<TransactionMsg>& mempool) {
    LocalBlock newBlock;
    newBlock.index = chain.size();
    newBlock.prevHash = chain.back().hash;
    
    // Miner reward / creation TX
    TransactionMsg cbTx;
    cbTx.setSenderAddress(myAddr.c_str());
    cbTx.setSenderPublicKey(CryptoUtils::getPublicKeyString(myKey).c_str());
    cbTx.setData(("CREATE:Oil_Batch_" + std::to_string(chain.size())).c_str());
    std::string r, s;
    CryptoUtils::signData(cbTx.getData(), myKey, r, s);
    cbTx.setSigR(r.c_str());
    cbTx.setSigS(s.c_str());
    cbTx.setTxId(CryptoUtils::sha256(cbTx.getData()).c_str());
    newBlock.transactions.push_back(cbTx);
    
    // Pull from mempool
    for (const auto& memTx : mempool) {
        newBlock.transactions.push_back(memTx);
    }
    
    newBlock.merkleRoot = calculateMerkleRoot(newBlock.transactions);
    newBlock.hash = CryptoUtils::sha256(std::to_string(newBlock.index) + newBlock.prevHash + newBlock.merkleRoot);
    
    return newBlock;
}

std::string BlockchainLedger::calculateMerkleRoot(std::vector<TransactionMsg>& txs) {
    if (txs.empty()) return "";
    std::vector<std::string> currentLevel;
    for(auto& tx : txs) currentLevel.push_back(CryptoUtils::sha256(tx.getData()));

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