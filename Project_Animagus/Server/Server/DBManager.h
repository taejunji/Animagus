#pragma once
#include "pch.h"


class DBManager
{
public:
    // Singleton pattern
    static DBManager& GetInstance() {
        static DBManager instance;
        return instance;
    }

    bool DBConnect();
    bool DBDisconnect();

    bool DBFindById(const char* id, const char* passwd, char* name, char* flag);
    bool DBLogOutById(const char* id);
    bool DBSignUp(const char* id, const char* passwd, const char* name);
    bool DBDeleteUserById(const char* id);

private:
    DBManager() = default;
    ~DBManager() { DBDisconnect(); }

    DBManager(const DBManager&) = delete;
    DBManager& operator=(const DBManager&) = delete;

private:
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt = 0;

};

void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
void show_error();
bool ConvertCharToWide(const char* src, wchar_t* dst, int dstMaxCount);
