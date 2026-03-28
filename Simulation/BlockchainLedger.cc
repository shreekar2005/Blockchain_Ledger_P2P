#include "BlockchainLedger.h"
#include "CryptoUtils.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

BlockchainLedger::BlockchainLedger() {}

void BlockchainLedger::createGenesis() {
    LocalBlock genesis;
    genesis.index = 0;
    genesis.prevHash = "0000000000000000000000000000000000000000000000000000000000000000";
    
    TransactionMsg tx;
    tx.setData("GENESIS:Initial_Supply");
    genesis.transactions.push_back(tx);

    genesis.merkleRoot = calculateMerkleRoot(genesis.transactions);
    genesis.hash = CryptoUtils::sha256(to_string(genesis.index) + genesis.prevHash + genesis.merkleRoot);
    
    chain.push_back(genesis);
    unspentAssets.insert("Initial_Supply");
}

int BlockchainLedger::getHeight() {
    return static_cast<int>(chain.size());
}

string BlockchainLedger::getHeadHash() {
    if (chain.empty()) return "";
    return chain.back().hash;
}

bool BlockchainLedger::validateTransaction(const TransactionMsg& tx) {
    bool isSignatureValid = CryptoUtils::verifySignature(
        tx.getData(), 
        tx.getSenderPublicKey(), 
        tx.getSigR(), 
        tx.getSigS()
    );

    if (!isSignatureValid) {
        return false;
    }

    string txData = tx.getData();
    if (txData.rfind("SPEND:", 0) == 0) {
        string assetId = txData.substr(6);
        if (unspentAssets.find(assetId) == unspentAssets.end()) {
            return false;
        }
    }

    return true;
}

int BlockchainLedger::validateAndAddBlock(BlockMsg *bMsg, double currentSimTime) {
    if (abs(currentSimTime - bMsg->getTimestamp()) > 3600.0) {
        return -1;
    }

    for (const auto& block : chain) {
        if (block.hash == bMsg->getCurrentHash()) {
            return 0;
        }
    }

    vector<string> assetsToSpend;
    vector<string> assetsToCreate;

    int transactionCount = bMsg->getTransactionsArraySize();
    for (int i = 0; i < transactionCount; ++i) {
        TransactionMsg tx = bMsg->getTransactions(i);
        if (!validateTransaction(tx)) {
            return -1;
        }

        string txData = tx.getData();
        if (txData.rfind("SPEND:", 0) == 0) {
            string assetToSpend = txData.substr(6);
            if (find(assetsToSpend.begin(), assetsToSpend.end(), assetToSpend) != assetsToSpend.end()) {
                return -1;
            }
            assetsToSpend.push_back(assetToSpend);
        } else if (txData.rfind("CREATE:", 0) == 0) {
            assetsToCreate.push_back(txData.substr(7));
        }
    }

    if (bMsg->getIndex() == static_cast<int>(chain.size())) {
        if (bMsg->getPreviousHash() != chain.back().hash) {
            return -1;
        }

        LocalBlock newBlock;
        newBlock.index = bMsg->getIndex();
        newBlock.hash = bMsg->getCurrentHash();
        newBlock.prevHash = bMsg->getPreviousHash();
        newBlock.merkleRoot = bMsg->getMerkleRoot();
        
        for (int i = 0; i < transactionCount; ++i) {
            newBlock.transactions.push_back(bMsg->getTransactions(i));
        }

        for (const auto& asset : assetsToSpend) {
            unspentAssets.erase(asset);
        }
        for (const auto& asset : assetsToCreate) {
            unspentAssets.insert(asset);
        }
        
        chain.push_back(newBlock);
        return 1;
    }

    return 0;
}

LocalBlock BlockchainLedger::createCandidateBlock(int myIndex, string myIp, EVP_PKEY* myKey, string myAddr, vector<TransactionMsg>& mempool) {
    LocalBlock newBlock;
    newBlock.index = static_cast<int>(chain.size());
    newBlock.prevHash = chain.back().hash;
    
    TransactionMsg coinbaseTx;
    coinbaseTx.setSenderAddress(myAddr.c_str());
    coinbaseTx.setSenderPublicKey(CryptoUtils::getPublicKeyString(myKey).c_str());
    
    string rewardData = "CREATE:Block_Reward_" + to_string(newBlock.index) + "_Node_" + to_string(myIndex);
    coinbaseTx.setData(rewardData.c_str());
    
    string sigR, sigS;
    CryptoUtils::signData(rewardData, myKey, sigR, sigS);
    coinbaseTx.setSigR(sigR.c_str());
    coinbaseTx.setSigS(sigS.c_str());
    coinbaseTx.setTxId(CryptoUtils::sha256(rewardData).c_str());
    
    newBlock.transactions.push_back(coinbaseTx);
    
    for (const auto& pendingTx : mempool) {
        newBlock.transactions.push_back(pendingTx);
    }
    
    newBlock.merkleRoot = calculateMerkleRoot(newBlock.transactions);
    newBlock.hash = CryptoUtils::sha256(to_string(newBlock.index) + newBlock.prevHash + newBlock.merkleRoot);
    
    return newBlock;
}

string BlockchainLedger::calculateMerkleRoot(vector<TransactionMsg>& txs) {
    if (txs.empty()) {
        return "";
    }

    vector<string> hashes;
    for (const auto& tx : txs) {
        hashes.push_back(CryptoUtils::sha256(tx.getData()));
    }

    while (hashes.size() > 1) {
        if (hashes.size() % 2 != 0) {
            hashes.push_back(hashes.back());
        }

        vector<string> nextLevel;
        for (size_t i = 0; i < hashes.size(); i += 2) {
            nextLevel.push_back(CryptoUtils::sha256(hashes[i] + hashes[i + 1]));
        }
        hashes = move(nextLevel);
    }

    return hashes[0];
}

BlockMsg* BlockchainLedger::getBlockAtHeight(int height) {
    if (height < 0 || height >= static_cast<int>(chain.size())) {
        return nullptr;
    }

    for (const auto& localBlock : chain) {
        if (localBlock.index == height) {
            BlockMsg* bMsg = new BlockMsg("HistoricalBlock");
            bMsg->setIndex(localBlock.index);
            bMsg->setPreviousHash(localBlock.prevHash.c_str());
            bMsg->setMerkleRoot(localBlock.merkleRoot.c_str());
            bMsg->setCurrentHash(localBlock.hash.c_str());
            
            size_t txCount = localBlock.transactions.size();
            bMsg->setTransactionsArraySize(txCount);
            for (size_t i = 0; i < txCount; ++i) {
                bMsg->setTransactions(i, localBlock.transactions[i]);
            }
            return bMsg;
        }
    }
    
    return nullptr;
}