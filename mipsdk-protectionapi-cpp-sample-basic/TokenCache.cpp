#include "TokenCache.h"
#include <fstream>

TokenCache::TokenCache() {
    mCacheFile = L"token_cache.txt";
    loadFromFile(mCacheFile);
}

TokenCache::TokenCache(const std::wstring& filename) : mCacheFile(filename) {
    loadFromFile(mCacheFile);
}

bool TokenCache::isTokenValid(const std::wstring& userId, const std::wstring& scope) const {
    auto userIt = tokenMap.find(userId);
    if (userIt != tokenMap.end()) {
        const auto& scopeMap = userIt->second;
        auto scopeIt = scopeMap.find(scope);
        if (scopeIt != scopeMap.end()) {
            auto now = std::chrono::system_clock::now();
            return now < scopeIt->second.expiry;
        }
    }
    return false;
}

std::wstring TokenCache::getToken(const std::wstring& userId, const std::wstring& scope) const {
    if (isTokenValid(userId, scope)) {
        return tokenMap.at(userId).at(scope).token;
    }
    return L"";
}

void TokenCache::updateToken(const std::wstring& userId, const std::wstring& scope,
    const std::wstring& token,
    std::chrono::system_clock::time_point expiry) {
    tokenMap[userId][scope] = { token, expiry };
    saveToFile();
}

bool TokenCache::loadFromFile(const std::wstring& filename) {
    std::wifstream inFile(filename);
    if (!inFile.is_open()) return false;

    std::wstring userId, scope, token, expiryLine;
    while (std::getline(inFile, userId) &&
        std::getline(inFile, scope) &&
        std::getline(inFile, token) &&
        std::getline(inFile, expiryLine)) {
        try {
            long long seconds = std::stoll(expiryLine);
            auto expiry = std::chrono::system_clock::time_point{ std::chrono::seconds{seconds} };
            tokenMap[userId][scope] = { token, expiry };
        }
        catch (...) {
            continue; // skip malformed entry
        }
    }

    return true;
}

bool TokenCache::saveToFile() const {
    if (!mCacheFile.empty()) {
        std::wofstream outFile(mCacheFile);
        if (!outFile.is_open()) return false;

        for (const auto& userPair : tokenMap) {
            const std::wstring& userId = userPair.first;
            for (const auto& scopePair : userPair.second) {
                const std::wstring& scope = scopePair.first;
                const TokenInfo& info = scopePair.second;

                outFile << userId << L'\n'
                    << scope << L'\n'
                    << info.token << L'\n'
                    << std::chrono::duration_cast<std::chrono::seconds>(
                        info.expiry.time_since_epoch())
                    .count()
                    << L'\n';
            }
        }
        return true;
    }

    return false;
}
