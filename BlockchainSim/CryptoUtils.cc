#include "CryptoUtils.h"
#include <sstream>
#include <iomanip>

std::string CryptoUtils::sha256(const std::string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

EC_KEY* CryptoUtils::generateKey() {
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_KEY_generate_key(key);
    return key;
}

std::string CryptoUtils::signData(const std::string data, EC_KEY* key) {
    // In a real system, we would sign the hash of data.
    // For this simulation, we return a dummy signature string.
    return "Sig_" + sha256(data).substr(0, 10);
}