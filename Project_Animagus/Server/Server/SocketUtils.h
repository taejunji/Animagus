#pragma once

class SocketUtils
{
public:
    static LPFN_CONNECTEX		ConnectEx;
    static LPFN_DISCONNECTEX	DisconnectEx;
    static LPFN_ACCEPTEX		AcceptEx;

    static void     Init();
    static SOCKET   CreateSocket();
    static void     Close(SOCKET& socket);

    //bool Bind(SOCKET socket);
    static bool     BindAnyAddress(SOCKET socket);

    static bool     Listen(SOCKET socket, int32 backlog = SOMAXCONN);
    static bool     BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);

    static bool     SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
    static bool     SetReuseAddress(SOCKET socket, bool flag);
    static bool     SetTcpNoDelay(SOCKET socket, bool flag);
    static bool     SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);    // ListenSocket의 특성을 ClientSocket에 그대로 적용
};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
    return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}
