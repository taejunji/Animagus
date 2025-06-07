#pragma once


class DBManager
{
public:
    DBManager() = default;
    ~DBManager() { DBDisconnect(); }
    
    bool DBConnect();
    bool DBDisconnect();

    bool DBFindById(const char* id, char* name, short* x, short* y);


private:
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt = 0;

};

void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
void show_error();
