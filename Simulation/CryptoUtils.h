// CryptoUtils.h

#ifndef CRYPTOUTILS_H_
#define CRYPTOUTILS_H_

#include <string>
#include <vector>
#include <openssl/evp.h>

/**
 * @brief helper class for cryptography functions
 */
class CryptoUtils {
public:
    /**
     * @brief generates 256 bit hash for given string data
     * @param str the input string to be hashed
     * @return the hexadecimal string representation of the hash
     */
    static std::string sha256(const std::string str);
    
    /**
     * @brief creates new elliptic curve key pair for node identity
     * @return pointer to the generated evp_pkey object
     */
    static EVP_PKEY* generateKey();
    
    /**
     * @brief creates mock signature for simulation purposes
     * @param data the string data to sign
     * @param key the private key used for signing (unused in mock)
     * @return a dummy signature string
     */
    static std::string signData(const std::string data, EVP_PKEY* key);
};

#endif