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
    bool DBSignUp(const char* id, const char* passwd, char* name);


private:
    TextDBManager() = default;
    ~TextDBManager() { DBDisconnect(); }

    TextDBManager(const TextDBManager&) = delete;
    TextDBManager& operator=(const TextDBManager&) = delete;

private:
    struct UserInfo {
        char id[21];
        char passwd[21];
        char name[Protocol::MAX_NAME_LEN + 1];
        bool is_login;
    };

    std::list<UserInfo> m_userList;
    std::mutex          m_mutex;
};

