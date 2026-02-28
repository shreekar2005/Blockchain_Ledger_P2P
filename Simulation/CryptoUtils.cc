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
#include <memory>

using namespace std;

string CryptoUtils::sha256(const string str) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;
    
    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> context(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    
    EVP_DigestInit_ex(context.get(), EVP_sha256(), nullptr);
    EVP_DigestUpdate(context.get(), str.c_str(), str.size());
    EVP_DigestFinal_ex(context.get(), hash, &lengthOfHash);

    stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; i++) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

EVP_PKEY* CryptoUtils::generateKey() {
    unsigned char randBytes[32];
    RAND_bytes(randBytes, 32);
    
    unsigned char secretKey[32];
    unsigned int skLen = 0;
    
    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> mdctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    EVP_DigestInit_ex(mdctx.get(), EVP_sha256(), nullptr);
    EVP_DigestUpdate(mdctx.get(), randBytes, 32);
    EVP_DigestFinal_ex(mdctx.get(), secretKey, &skLen);

    BIGNUM* privBn = BN_bin2bn(secretKey, 32, nullptr);
    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_KEY_set_private_key(ecKey, privBn);

    const EC_GROUP* group = EC_KEY_get0_group(ecKey);
    EC_POINT* pubPoint = EC_POINT_new(group);
    EC_POINT_mul(group, pubPoint, privBn, nullptr, nullptr, nullptr);
    EC_KEY_set_public_key(ecKey, pubPoint);

    EVP_PKEY* pkey = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(pkey, ecKey);

    BN_free(privBn);
    EC_POINT_free(pubPoint);
    
    return pkey;
}

string CryptoUtils::getPublicKeyString(EVP_PKEY* key) {
    int len = i2d_PUBKEY(key, nullptr);
    vector<unsigned char> pubKeyBytes(len);
    unsigned char* p = pubKeyBytes.data();
    i2d_PUBKEY(key, &p);
    
    stringstream ss;
    for (int i = 0; i < len; i++) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(pubKeyBytes[i]);
    }
    return ss.str();
}

string CryptoUtils::deriveWalletAddress(EVP_PKEY* key) {
    int len = i2d_PUBKEY(key, nullptr);
    vector<unsigned char> pubKeyBytes(len);
    unsigned char* p = pubKeyBytes.data();
    i2d_PUBKEY(key, &p);

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen = 0;
    
    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> context(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    EVP_DigestInit_ex(context.get(), EVP_sha256(), nullptr);
    EVP_DigestUpdate(context.get(), pubKeyBytes.data(), len);
    EVP_DigestFinal_ex(context.get(), hash, &hashLen);

    stringstream ss;
    ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[30]);
    ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[31]);
    
    return "0x" + ss.str();
}

bool CryptoUtils::signData(const string data, EVP_PKEY* key, string& r, string& s) {
    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    size_t sigLen = 0;

    if (EVP_DigestSignInit(ctx.get(), nullptr, EVP_sha256(), nullptr, key) <= 0) return false;
    if (EVP_DigestSignUpdate(ctx.get(), data.c_str(), data.size()) <= 0) return false;
    if (EVP_DigestSignFinal(ctx.get(), nullptr, &sigLen) <= 0) return false;

    vector<unsigned char> sig(sigLen);
    if (EVP_DigestSignFinal(ctx.get(), sig.data(), &sigLen) <= 0) return false;

    const unsigned char* p = sig.data();
    ECDSA_SIG* ecdsaSig = d2i_ECDSA_SIG(nullptr, &p, sigLen);
    if (!ecdsaSig) return false;

    const BIGNUM *r_bn, *s_bn;
    ECDSA_SIG_get0(ecdsaSig, &r_bn, &s_bn);
    
    char *r_hex = BN_bn2hex(r_bn);
    char *s_hex = BN_bn2hex(s_bn);
    
    r = string(r_hex);
    s = string(s_hex);

    OPENSSL_free(r_hex);
    OPENSSL_free(s_hex);
    ECDSA_SIG_free(ecdsaSig);
    
    return true;
}

bool CryptoUtils::verifySignature(const string data, const string pubKeyStr, const string r, const string s) {
    vector<unsigned char> pubKeyBytes;
    for (size_t i = 0; i < pubKeyStr.length(); i += 2) {
        unsigned int byteValue;
        stringstream ss;
        ss << hex << pubKeyStr.substr(i, 2);
        ss >> byteValue;
        pubKeyBytes.push_back(static_cast<unsigned char>(byteValue));
    }

    const unsigned char* p = pubKeyBytes.data();
    EVP_PKEY* key = d2i_PUBKEY(nullptr, &p, pubKeyBytes.size());
    if (!key) return false;

    BIGNUM *r_bn = nullptr, *s_bn = nullptr;
    if (BN_hex2bn(&r_bn, r.c_str()) <= 0 || BN_hex2bn(&s_bn, s.c_str()) <= 0) {
        if (r_bn) BN_free(r_bn);
        if (s_bn) BN_free(s_bn);
        EVP_PKEY_free(key);
        return false;
    }

    ECDSA_SIG* ecdsaSig = ECDSA_SIG_new();
    ECDSA_SIG_set0(ecdsaSig, r_bn, s_bn);

    unsigned char* sigDer = nullptr;
    int sigLen = i2d_ECDSA_SIG(ecdsaSig, &sigDer);

    unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> v_ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    bool isValid = false;

    if (EVP_DigestVerifyInit(v_ctx.get(), nullptr, EVP_sha256(), nullptr, key) > 0) {
        if (EVP_DigestVerifyUpdate(v_ctx.get(), data.c_str(), data.size()) > 0) {
            if (EVP_DigestVerifyFinal(v_ctx.get(), sigDer, sigLen) == 1) {
                isValid = true;
            }
        }
    }

    OPENSSL_free(sigDer);
    ECDSA_SIG_free(ecdsaSig);
    EVP_PKEY_free(key);
    
    return isValid;
}
