#pragma once
#include "pch.h"
#include "protocol.h"

class TextDBManager
{
public:
    // Singleton pattern
    static TextDBManager& GetInstance() {
        static TextDBManager instance;
        return instance;
    }

    bool DBConnect();
    bool DBDisconnect();

    bool DBFindById(const char* id, const char* passwd, char* name, char* flag);
    bool DBLogOutById(const char* id);
    bool DBSignUp(const char* id, const char* passwd, const char* name);
    bool DBDeleteUserById(const char* id);


private:
    TextDBManager() = default;
    ~TextDBManager() { DBDisconnect(); }

    TextDBManager(const TextDBManager&) = delete;
    TextDBManager& operator=(const TextDBManager&) = delete;

private:
    struct UserInfo {
        std::string id;
        std::string password;
        std::string name;
        bool        is_login;
    };

    std::unordered_map<std::string/*id*/, UserInfo> m_userList;
    std::mutex                                      m_mutex;
};

