// task2.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

/**
 * @brief computes 256 bit sha hash for given string and stores it in output buffer
 * @param input the string data to hash
 * @param output the buffer where the hex string hash will be written
 */
void compute_sha256(const char *input, char *output) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int length = 0;
    
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, input, strlen(input));
    EVP_DigestFinal_ex(ctx, hash, &length);
    EVP_MD_CTX_free(ctx);

    for (unsigned int i = 0; i < length; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0';
}

/**
 * @brief concatenates two string hashes and computes their combined hash
 * @param hash1 the left child hash
 * @param hash2 the right child hash
 * @param output the buffer for the resulting parent hash
 */
void combine_and_hash(const char *hash1, const char *hash2, char *output) {
    char combined[129]; 
    snprintf(combined, sizeof(combined), "%s%s", hash1, hash2);
    compute_sha256(combined, output);
}

int main() {
    const char *txs[8] = {"tx1", "tx2", "tx3", "tx4", "tx5", "tx6", "tx7", "tx8"};
    
    char leaves[8][65]; // array to store 8 leaf hashes
    char l1[4][65];     // array to store 4 level-1 hashes
    char l2[2][65];     // array to store 2 level-2 hashes
    char root[65];      // root hash of the merkle tree

    for (int i = 0; i < 8; i++) {
        compute_sha256(txs[i], leaves[i]);
    }

    for (int i = 0; i < 4; i++) {
        combine_and_hash(leaves[2*i], leaves[2*i+1], l1[i]);
    }

    for (int i = 0; i < 2; i++) {
        combine_and_hash(l1[2*i], l1[2*i+1], l2[i]);
    }

    combine_and_hash(l2[0], l2[1], root);

    printf("merkle root: %s\n\n", root);

    printf("Q) How does a merkle proof work for verifying a single transaction?\n");
    printf("to prove tx4 is in the block, we only need these 3 hashes:\n");
    printf("1. sibling of tx4 -> hash(tx3): %s\n", leaves[2]);
    printf("2. sibling of hash(tx3+tx4) -> hash(tx1+tx2): %s\n", l1[0]);
    printf("3. sibling of left half -> hash(tx5..tx8): %s\n\n", l2[1]);

    printf("Q) Why is providing a merkle proof more efficient for a light node?\n");
    printf("a light node (spv) does not need to download the entire block body containing thousands of transactions.\n");
    printf("by using a merkle proof, the verification scales logarithmically (o(log2 n)).\n");
    printf("for 8 transactions, the node only needs 3 hashes to verify one transaction instead of downloading all 8.\n");
    printf("for 65,536 transactions, it would only need 16 hashes, saving massive amounts of bandwidth, computation, and storage.\n");

    return 0;
}