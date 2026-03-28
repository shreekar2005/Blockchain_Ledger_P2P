// task1.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

/**
 * @brief computes 256 bit sha hash for a string and returns raw bytes
 */
void compute_sha256_raw(const char *input, unsigned char *output) {
    unsigned int length = 0;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, input, strlen(input));
    EVP_DigestFinal_ex(ctx, output, &length);
    EVP_MD_CTX_free(ctx);
}

/**
 * @brief converts raw byte hash to hex string for printing
 */
void bytes_to_hex(const unsigned char *hash, char *hex_output, int length) {
    for (int i = 0; i < length; i++) {
        sprintf(hex_output + (i * 2), "%02x", hash[i]);
    }
    hex_output[length * 2] = '\0';
}

/**
 * @brief counts how many bits are different between two raw hashes
 */
int count_bit_difference(const unsigned char *hash1, const unsigned char *hash2, int length) {
    int diff_count = 0;
    for (int i = 0; i < length; i++) {
        unsigned char xor_val = hash1[i] ^ hash2[i];
        // count the number of 1s in the xor result (which represent flipped bits)
        while (xor_val > 0) {
            diff_count += xor_val & 1;
            xor_val >>= 1;
        }
    }
    return diff_count;
}

int main() {
    // standard transaction
    const char *tx_original = "Alice pays Bob 100 barrels of oil";
    
    // exact same transaction, but changed '1' to '2' (1 character difference)
    const char *tx_modified = "Alice pays Bob 200 barrels of oil";

    unsigned char raw_hash1[EVP_MAX_MD_SIZE];
    unsigned char raw_hash2[EVP_MAX_MD_SIZE];
    char hex_hash1[65];
    char hex_hash2[65];

    // compute hashes
    compute_sha256_raw(tx_original, raw_hash1);
    compute_sha256_raw(tx_modified, raw_hash2);

    // convert to hex strings for display
    bytes_to_hex(raw_hash1, hex_hash1, 32);
    bytes_to_hex(raw_hash2, hex_hash2, 32);

    // calculate avalanche effect
    int changed_bits = count_bit_difference(raw_hash1, raw_hash2, 32);
    float percentage = ((float)changed_bits / 256.0) * 100.0;

    printf("original tx : \"%s\"\n", tx_original);
    printf("hash 1      : %s\n\n", hex_hash1);
    
    printf("modified tx : \"%s\" (changed '1' to '2')\n", tx_modified);
    printf("hash 2      : %s\n\n", hex_hash2);

    printf("bits changed: %d out of 256 bits\n", changed_bits);
    printf("percentage  : %.2f%%\n\n", percentage);

    printf("Q) What percentage of the bits changed in the second hash?\n");
    printf("approximately %.2f%% of the bits changed. in a proper cryptographic hash function like sha-256, changing even a single character in the input will completely scramble the output, typically flipping around 50%% of the bits. this is known as the strict avalanche criterion.\n\n", percentage);

    printf("Q) How does this property support the tamper-resistance of the blockchain?\n");
    printf("it makes silent tampering mathematically impossible. if a hacker tries to alter a past transaction (e.g., changing 100 barrels to 200), the block's hash changes entirely. because the next block stores this hash, the link is broken. the hacker would have to re-mine the altered block and every single block after it before the rest of the network moves forward, which requires unfeasible amounts of computational power.\n");

    return 0;
}