// CryptoUtils.cc

#include "CryptoUtils.h"
#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <sstream>
#include <iomanip>

std::string CryptoUtils::sha256(const std::string str) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    EVP_MD_CTX* context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_sha256(), nullptr);
    EVP_DigestUpdate(context, str.c_str(), str.size());
    EVP_DigestFinal_ex(context, hash, &lengthOfHash);
    EVP_MD_CTX_free(context);

    std::stringstream ss;
    for(unsigned int i = 0; i < lengthOfHash; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

EVP_PKEY* CryptoUtils::generateKey() {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    EVP_PKEY *key = nullptr;
    
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_secp256k1);
    EVP_PKEY_keygen(ctx, &key);
    
    EVP_PKEY_CTX_free(ctx);
    return key;
}

std::string CryptoUtils::signData(const std::string data, EVP_PKEY* key) {
    return "Sig_" + sha256(data).substr(0, 10);
}