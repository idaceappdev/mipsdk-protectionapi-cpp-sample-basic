#pragma once
#include <string>
#include <unordered_map>
#include <chrono>

struct TokenInfo {
    std::wstring token;
    std::chrono::system_clock::time_point expiry;
};

class TokenCache {
public:
    // Check if token exists and is valid for a given user and scope
    bool isTokenValid(const std::wstring& userId, const std::wstring& scope) const;

    // Retrieve token if valid; returns empty wstring otherwise
    std::wstring getToken(const std::wstring& userId, const std::wstring& scope) const;

    // Add or update token for a given user and scope
    void updateToken(const std::wstring& userId, const std::wstring& scope,
        const std::wstring& token,
        std::chrono::system_clock::time_point expiry);

    // Load token cache from file
    bool loadFromFile(const std::wstring& filename);

    // Save token cache to file
    bool saveToFile() const;

    TokenCache();
    TokenCache(const std::wstring& filename);

private:
    std::unordered_map<std::wstring, std::unordered_map<std::wstring, TokenInfo>> tokenMap;
    std::wstring mCacheFile;
};
