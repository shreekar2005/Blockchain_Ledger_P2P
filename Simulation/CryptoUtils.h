/**
 * @file CryptoUtils.h
 * @brief Provides cryptographic utility functions for hashing, key management, and digital signatures.
 */

#ifndef CRYPTO_UTILS_H_
#define CRYPTO_UTILS_H_

#include <string>
#include <openssl/evp.h>
#include <openssl/ecdsa.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>

using namespace std;

/**
 * @class CryptoUtils
 * @brief A utility class for cryptographic operations used within the blockchain.
 */
class CryptoUtils {
public:
    /**
     * @brief Computes the SHA-256 hash of a given string.
     * @param str The input string to hash.
     * @return The resulting SHA-256 hash as a hex string.
     */
    static string sha256(const string str);

    /**
     * @brief Generates a new EVP_PKEY containing an ECDSA key pair.
     * @return A pointer to the generated EVP_PKEY, or nullptr on failure.
     */
    static EVP_PKEY* generateKey();

    /**
     * @brief Derives a wallet address from a given EVP_PKEY.
     * @param key The EVP_PKEY to derive the address from.
     * @return The wallet address as a string.
     */
    static string deriveWalletAddress(EVP_PKEY* key);

    /**
     * @brief Extracts the public key as a string from an EVP_PKEY.
     * @param key The EVP_PKEY to extract from.
     * @return The public key encoded as a string.
     */
    static string getPublicKeyString(EVP_PKEY* key);
    
    /**
     * @brief Signs data using an ECDSA private key.
     * @param data The input string to sign.
     * @param key The EVP_PKEY containing the private key.
     * @param r Output parameter for the 'r' component of the signature.
     * @param s Output parameter for the 's' component of the signature.
     * @return True if signing was successful, false otherwise.
     */
    static bool signData(const string data, EVP_PKEY* key, string& r, string& s);

    /**
     * @brief Verifies an ECDSA signature.
     * @param data The original data that was signed.
     * @param pubKeyStr The public key of the signer as a string.
     * @param r The 'r' component of the signature.
     * @param s The 's' component of the signature.
     * @return True if the signature is valid, false otherwise.
     */
    static bool verifySignature(const string data, const string pubKeyStr, const string r, const string s);
};

#endif // CRYPTO_UTILS_H_
