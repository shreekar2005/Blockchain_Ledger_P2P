#include "CryptoUtils.h"
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/ecdsa.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <vector>

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
    // Requirement: Random number -> SHA256 -> Secret Key (sk)
    unsigned char randBytes[32];
    RAND_bytes(randBytes, 32);
    
    unsigned char sk[32];
    unsigned int skLen = 0;
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(mdctx, randBytes, 32);
    EVP_DigestFinal_ex(mdctx, sk, &skLen);
    EVP_MD_CTX_free(mdctx);

    BIGNUM* privBn = BN_bin2bn(sk, 32, nullptr);
    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_KEY_set_private_key(ecKey, privBn);

    // Derive Public Key: pk = sk * G (Double-and-Add internally)
    EC_GROUP* group = (EC_GROUP*)EC_KEY_get0_group(ecKey);
    EC_POINT* pubPoint = EC_POINT_new(group);
    EC_POINT_mul(group, pubPoint, privBn, nullptr, nullptr, nullptr);
    EC_KEY_set_public_key(ecKey, pubPoint);

    EVP_PKEY* pkey = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(pkey, ecKey);

    BN_free(privBn);
    EC_POINT_free(pubPoint);
    return pkey;
}

std::string CryptoUtils::getPublicKeyString(EVP_PKEY* key) {
    int len = i2d_PUBKEY(key, nullptr);
    unsigned char* pubKeyBytes = new unsigned char[len];
    unsigned char* p = pubKeyBytes;
    i2d_PUBKEY(key, &p);
    
    std::stringstream ss;
    for(int i = 0; i < len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)pubKeyBytes[i];
    }
    delete[] pubKeyBytes;
    return ss.str();
}

std::string CryptoUtils::deriveWalletAddress(EVP_PKEY* key) {
    // Requirement: SHA-256 of binary Public Key, last 16 bits in 4-digit hex
    int len = i2d_PUBKEY(key, nullptr);
    unsigned char* pubKeyBytes = new unsigned char[len];
    unsigned char* p = pubKeyBytes;
    i2d_PUBKEY(key, &p);

    unsigned char hash[32];
    unsigned int hashLen = 0;
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_sha256(), nullptr);
    EVP_DigestUpdate(context, pubKeyBytes, len);
    EVP_DigestFinal_ex(context, hash, &hashLen);
    EVP_MD_CTX_free(context);
    delete[] pubKeyBytes;

    std::stringstream ss;
    // Last 16 bits = last 2 bytes of the 32-byte hash
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[30];
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[31];
    
    return "0x" + ss.str();
}

bool CryptoUtils::signData(const std::string data, EVP_PKEY* key, std::string& r, std::string& s) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    size_t sigLen = 0;
    if (EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, key) <= 0) {
        EVP_MD_CTX_free(ctx); return false;
    }
    if (EVP_DigestSignUpdate(ctx, data.c_str(), data.size()) <= 0) {
        EVP_MD_CTX_free(ctx); return false;
    }
    if (EVP_DigestSignFinal(ctx, nullptr, &sigLen) <= 0) {
        EVP_MD_CTX_free(ctx); return false;
    }

    unsigned char* sig = (unsigned char*)OPENSSL_malloc(sigLen);
    if (EVP_DigestSignFinal(ctx, sig, &sigLen) <= 0) {
        OPENSSL_free(sig); EVP_MD_CTX_free(ctx); return false;
    }

    const unsigned char* p = sig;
    ECDSA_SIG* ecdsaSig = d2i_ECDSA_SIG(nullptr, &p, sigLen);
    if (!ecdsaSig) {
        OPENSSL_free(sig); EVP_MD_CTX_free(ctx); return false;
    }

    const BIGNUM *r_bn, *s_bn;
    ECDSA_SIG_get0(ecdsaSig, &r_bn, &s_bn);
    char *r_hex = BN_bn2hex(r_bn);
    char *s_hex = BN_bn2hex(s_bn);
    r = std::string(r_hex);
    s = std::string(s_hex);

    OPENSSL_free(r_hex); OPENSSL_free(s_hex);
    ECDSA_SIG_free(ecdsaSig); OPENSSL_free(sig);
    EVP_MD_CTX_free(ctx);
    return true;
}

bool CryptoUtils::verifySignature(const std::string data, const std::string pubKeyStr, const std::string r, const std::string s) {
    std::vector<unsigned char> pubKeyBytes;
    for (size_t i = 0; i < pubKeyStr.length(); i += 2) {
        unsigned int byte;
        std::stringstream ss;
        ss << std::hex << pubKeyStr.substr(i, 2);
        ss >> byte;
        pubKeyBytes.push_back(static_cast<unsigned char>(byte));
    }
    const unsigned char* p = pubKeyBytes.data();
    EVP_PKEY* key = d2i_PUBKEY(nullptr, &p, pubKeyBytes.size());
    if (!key) return false;

    BIGNUM *r_bn = nullptr, *s_bn = nullptr;
    if (BN_hex2bn(&r_bn, r.c_str()) <= 0 || BN_hex2bn(&s_bn, s.c_str()) <= 0) {
        if (r_bn) BN_free(r_bn); if (s_bn) BN_free(s_bn);
        EVP_PKEY_free(key); return false;
    }
    ECDSA_SIG* ecdsaSig = ECDSA_SIG_new();
    ECDSA_SIG_set0(ecdsaSig, r_bn, s_bn);

    unsigned char* sigDer = nullptr;
    int sigLen = i2d_ECDSA_SIG(ecdsaSig, &sigDer);

    EVP_MD_CTX* v_ctx = EVP_MD_CTX_new();
    bool valid = false;
    if (EVP_DigestVerifyInit(v_ctx, nullptr, EVP_sha256(), nullptr, key) > 0) {
        if (EVP_DigestVerifyUpdate(v_ctx, data.c_str(), data.size()) > 0) {
            if (EVP_DigestVerifyFinal(v_ctx, sigDer, sigLen) == 1) valid = true;
        }
    }

    OPENSSL_free(sigDer); EVP_MD_CTX_free(v_ctx);
    ECDSA_SIG_free(ecdsaSig); EVP_PKEY_free(key);
    return valid;
}
