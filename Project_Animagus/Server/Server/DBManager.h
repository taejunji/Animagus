#pragma once
#include "pch.h"


class DBManager
{
public:
    DBManager() = default;
    ~DBManager() { DBDisconnect(); }
    
    bool DBConnect();
    bool DBDisconnect();

    bool DBFindById(const char* id, const char* passwd, char* name);
    bool DBSignUp(const char* id, const char* passwd, char* name);

private:
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt = 0;

};

void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
void show_error();
