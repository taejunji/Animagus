#include "DBManager.h"
#include "pch.h"
#include "protocol.h"

void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
    SQLSMALLINT iRec = 0;
    SQLINTEGER iError;
    WCHAR wszMessage[1000];
    WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
    if (RetCode == SQL_INVALID_HANDLE) {
        fwprintf(stderr, L"Invalid handle!\n");
        return;
    }
    while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
        (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
        // Hide data truncated..
        if (wcsncmp(wszState, L"01004", 5)) {
            fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
        }
    }
}

void show_error()
{
    printf("error\n");
}

bool ConvertCharToWide(const char* src, wchar_t* dst, int dstMaxCount)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, dstMaxCount);
    return (len > 0 && len <= dstMaxCount);
}


bool DBManager::DBConnect()
{
    SQLRETURN retcode;

    // Allocate environment handle  
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        std::wcerr << L"SQLAllocHandle ENV failed\n";
        return false;
    }

    // Set the ODBC version environment attribute  
    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        std::wcerr << L"SQLSetEnvAttr failed\n";
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        henv = SQL_NULL_HENV;
        return false;
    }

    // Allocate connection handle  
    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        std::wcerr << L"SQLAllocHandle DBC failed\n";
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        henv = SQL_NULL_HENV;
        return false;
    }

    // Set login timeout to 5 seconds  
    SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

    // Connect to data source  
    retcode = SQLConnect(hdbc, (SQLWCHAR*)L"AnimagusODBC", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        std::wcerr << L"SQLConnect failed\n";
        HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        hdbc = SQL_NULL_HDBC;
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
        henv = SQL_NULL_HENV;
        return false;
    }

    std::cout << "[DB] Connection Success" << std::endl;
    return true;
}

bool DBManager::DBDisconnect()
{

    return true;
}

bool DBManager::DBFindById(const char* id, const char* passwd, char* name, char* flag)
{
    if (hdbc == SQL_NULL_HDBC) {
        return false;
    }

    SQLRETURN retcode;
    SQLWCHAR user_name[Protocol::MAX_NAME_LEN + 1];
    SQLCHAR Logged;
    SQLLEN cbId, cbPwd, cbName, cbLog;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    const wchar_t* sqlQuery = L"{CALL find_and_login_user(?, ?)}";
    retcode = SQLPrepare(hstmt, (SQLWCHAR*)sqlQuery, SQL_NTS);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }
    
    // Bind params ( user_id , user_passwd )
    wchar_t wid[21] = { 0 };
    ConvertCharToWide(id, wid, 21);
    cbId = SQL_NTS;
    retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        20, 0, (SQLPOINTER)wid, sizeof(wid), &cbId);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    wchar_t wpwd[21] = { 0 };
    ConvertCharToWide(passwd, wpwd, 21);
    cbPwd = SQL_NTS;
    retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        20, 0, (SQLPOINTER)wpwd, sizeof(wpwd), &cbPwd);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    // Bind columns ( user_name , is_login )
    retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, user_name, sizeof(user_name), &cbName);
    retcode = SQLBindCol(hstmt, 2, SQL_C_BIT, &Logged, sizeof(Logged), &cbLog);

    retcode = SQLExecute(hstmt);
    if (!SQL_SUCCEEDED(retcode)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return false;
    }


    retcode = SQLFetch(hstmt);
    if (retcode == SQL_NO_DATA) {
        // 아이디가 없음
        *flag = Protocol::LOGIN_NOEX;
        return false;
    }
    else if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        int needed = WideCharToMultiByte(CP_UTF8, 0, user_name, -1, nullptr, 0, nullptr, nullptr);
        if (needed > Protocol::MAX_NAME_LEN + 1) {
            *flag = Protocol::LOGIN_ERR;
            return false;
        }
        if (1 == Logged) {
            // 누군가 로그인 중
            *flag = Protocol::LOGIN_USING;
            return false;
        }
        WideCharToMultiByte(CP_UTF8, 0, user_name, -1, name, needed, nullptr, nullptr);
        return true;
    }
    else {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    return true;
}

bool DBManager::DBLogOutById(const char* id)
{
    if (hdbc == SQL_NULL_HDBC) {
        return false;
    }

    SQLRETURN retcode;
    SQLLEN    cbId;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    const wchar_t* sqlQuery = L"{CALL user_logout(?)}";
    retcode = SQLPrepare(hstmt, (SQLWCHAR*)sqlQuery, SQL_NTS);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    wchar_t wid[21] = { 0 };
    ConvertCharToWide(id, wid, 21);
    cbId = SQL_NTS;
    retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        20, 0, (SQLPOINTER)wid, sizeof(wid), &cbId);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    retcode = SQLExecute(hstmt);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    return true;
}

bool DBManager::DBSignUp(const char* id, const char* passwd, const char* name)
{
    if (hdbc == SQL_NULL_HDBC) {
        return false;
    }

    SQLRETURN retcode;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    static const wchar_t* sql = L"{CALL dbo.sign_up_user(?, ?, ?)}";
    retcode = SQLPrepare(hstmt, (SQLWCHAR*)sql, SQL_NTS);
    if (!SQL_SUCCEEDED(retcode)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return false;
    }

    wchar_t wid[21] = { 0 };
    ConvertCharToWide(id, wid, 21);
    SQLLEN cbId = SQL_NTS;
    retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        20, 0, (SQLPOINTER)wid, sizeof(wid), &cbId);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    wchar_t wpasswd[21] = { 0 };
    ConvertCharToWide(passwd, wpasswd, 21);
    SQLLEN cbPasswd = SQL_NTS;
    retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        20, 0, (SQLPOINTER)wpasswd, sizeof(wpasswd), &cbPasswd);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    wchar_t wname[21] = { 0 };
    ConvertCharToWide(name, wname, 21);
    SQLLEN cbName = SQL_NTS;
    retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        20, 0, (SQLPOINTER)wname, sizeof(wname), &cbName);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    retcode = SQLExecute(hstmt);
    if (!SQL_SUCCEEDED(retcode)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return false;
    }

    return true;
}

bool DBManager::DBDeleteUserById(const char* id)
{
    if (hdbc == SQL_NULL_HDBC) {
        return false;
    }

    SQLRETURN retcode;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(retcode)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    static const wchar_t* sql = L"{CALL dbo.delete_user_by_id(?)}";
    retcode = SQLPrepare(hstmt, (SQLWCHAR*)sql, SQL_NTS);
    if (!SQL_SUCCEEDED(retcode)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return false;
    }

    wchar_t wid[21] = { 0 };
    ConvertCharToWide(id, wid, 21);
    SQLLEN cbId = SQL_NTS;
    retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        20, 0, (SQLPOINTER)wid, sizeof(wid), &cbId);
    if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        return false;
    }

    retcode = SQLExecute(hstmt);
    if (!SQL_SUCCEEDED(retcode)) {
        HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        return false;
    }

    return true;
}

