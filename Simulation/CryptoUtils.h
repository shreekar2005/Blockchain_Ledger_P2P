#ifndef CRYPTOUTILS_H_
#define CRYPTOUTILS_H_

#include <string>
#include <openssl/evp.h>
#include <openssl/ecdsa.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>

class CryptoUtils {
public:
    static std::string sha256(const std::string str);
    static EVP_PKEY* generateKey();
    static std::string deriveWalletAddress(EVP_PKEY* key);
    static std::string getPublicKeyString(EVP_PKEY* key);
    
    // ECDSA Functions - Now using true OpenSSL implementation
    static bool signData(const std::string data, EVP_PKEY* key, std::string& r, std::string& s);
    static bool verifySignature(const std::string data, const std::string pubKeyStr, const std::string r, const std::string s);
};

#endif