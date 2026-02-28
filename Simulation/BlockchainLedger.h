/**
 * @file BlockchainLedger.h
 * @brief Manages the local copy of the blockchain and its state.
 */

#ifndef BLOCKCHAIN_LEDGER_H_
#define BLOCKCHAIN_LEDGER_H_

#include <vector>
#include <string>
#include <set>
#include <openssl/evp.h>
#include "Blockchain_m.h"

using namespace std;

/**
 * @struct LocalBlock
 * @brief Represents a block within the node's local blockchain storage.
 */
struct LocalBlock {
    int index;                                    ///< Block height/index
    string prevHash;                         ///< Hash of the previous block
    string merkleRoot;                       ///< Merkle tree root of transactions
    string hash;                             ///< Hash of the current block
    vector<TransactionMsg> transactions;     ///< List of transactions in this block
};

/**
 * @class BlockchainLedger
 * @brief Handles blockchain operations like block creation, validation, and chain state.
 */
class BlockchainLedger {
private:
    vector<LocalBlock> chain;                ///< The actual blockchain sequence
    set<string> unspentAssets;          ///< Set of current unspent transaction outputs

public:
    /**
     * @brief Constructor for BlockchainLedger.
     */
    BlockchainLedger();

    /**
     * @brief Initializes the blockchain with a genesis block.
     */
    void createGenesis();

    /**
     * @brief Creates a new candidate block from pending transactions in the mempool.
     * @param myIndex The index/ID of the creating node.
     * @param myIp IP address of the node.
     * @param myKey Private key used for signing (if applicable).
     * @param myAddr Wallet address of the node.
     * @param mempool The collection of pending transactions to include in the block.
     * @return A newly constructed LocalBlock.
     */
    LocalBlock createCandidateBlock(int myIndex, string myIp, EVP_PKEY* myKey, string myAddr, vector<TransactionMsg>& mempool);

    /**
     * @brief Validates a received block message and adds it to the chain if valid.
     * @param bMsg The received block message.
     * @param currentSimTime Current simulation timestamp for logging/validation.
     * @return Result code (e.g., 0 for success, non-zero for error).
     */
    int validateAndAddBlock(BlockMsg *bMsg, double currentSimTime);

    /**
     * @brief Gets the current height (length) of the blockchain.
     * @return The number of blocks in the chain.
     */
    int getHeight();

    /**
     * @brief Retrieves the hash of the most recent block in the chain.
     * @return The head block's hash string.
     */
    string getHeadHash();

    /**
     * @brief Calculates the Merkle Root for a set of transactions.
     * @param txs Vector of transactions to hash.
     * @return The resulting Merkle Root hash.
     */
    string calculateMerkleRoot(vector<TransactionMsg>& txs);

    /**
     * @brief Validates an individual transaction against current ledger state.
     * @param tx The transaction to validate.
     * @return True if valid, false otherwise.
     */
    bool validateTransaction(const TransactionMsg& tx);
};

#endif // BLOCKCHAIN_LEDGER_H_
