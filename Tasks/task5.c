#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <time.h>

/**
 * @brief computes 256 bit sha hash for given string and stores it in output buffer
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
 * @brief generates a random 32-character hex nonce
 */
void generate_nonce(char *nonce) {
    for (int i = 0; i < 32; i++) {
        sprintf(nonce + i, "%x", rand() % 16);
    }
    nonce[32] = '\0';
}

int main() {
    srand(time(NULL));

    // producer's secret delivery volume (m)
    const char *volume_m = "1000_barrels";
    
    // generate a random cryptographic nonce (r)
    char nonce_r[33];
    generate_nonce(nonce_r);

    // concatenate m || r
    char combined_mr[128];
    snprintf(combined_mr, sizeof(combined_mr), "%s%s", volume_m, nonce_r);

    // Phase 1: Exploration
    char commitment_C[65];
    compute_sha256(combined_mr, commitment_C);

    printf("phase 1: exploration (commitment phase)\n");
    printf("producer generates secret volume: %s\n", volume_m);
    printf("producer generates random nonce : %s\n", nonce_r);
    printf("producer broadcasts commitment c: %s\n", commitment_C);
    printf("(refiner only sees 'c'. the volume is hidden.)\n\n");

    // Phase 2: Refining
    printf("phase 2: refining (reveal and verify phase)\n");
    printf("producer reveals volume m: %s\n", volume_m);
    printf("producer reveals nonce r : %s\n", nonce_r);

    // refiner computes hash(m || r)
    char refiner_check[65];
    char refiner_combined[128];
    snprintf(refiner_combined, sizeof(refiner_combined), "%s%s", volume_m, nonce_r);
    compute_sha256(refiner_combined, refiner_check);

    printf("refiner computes hash(m || r)   : %s\n", refiner_check);

    if (strcmp(commitment_C, refiner_check) == 0) {
        printf("result: verification successful! the commitment matches.\n\n");
    } else {
        printf("result: verification failed! data was tampered.\n\n");
    }

    printf("Q) How do the Hiding and Binding properties of this scheme resolve coordination disputes between mutually distrustful parties?\n");
    printf("1. hiding property: during the exploration phase, the refiner only receives the hash (C). because of the pre-image resistance of sha-256 and the addition of the random nonce (r), the refiner cannot reverse-engineer the hash to discover the volume (m). this prevents the refiner from exploiting early knowledge of the producer's supply volume during market negotiations.\n");
    printf("2. binding property: once C is published on the blockchain, the producer is 'bound' to it. they cannot suddenly claim they committed to 500 barrels instead of 1000, because finding a new volume (m') and a new nonce (r') that hashes to the exact same commitment C is computationally infeasible (collision resistance). this protects the refiner from the producer backing out of agreed numbers.\n");

    return 0;
}