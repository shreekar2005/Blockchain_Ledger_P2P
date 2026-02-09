#ifndef CRYPTOUTILS_H_
#define CRYPTOUTILS_H_

#include <string>
#include <vector>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

class CryptoUtils {
public:
    static std::string sha256(const std::string str);
    static EC_KEY* generateKey();
    static std::string signData(const std::string data, EC_KEY* key); // Simulated signature
};

#endif