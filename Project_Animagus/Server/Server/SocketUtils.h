#pragma once

namespace SocketUtils
{
    static LPFN_CONNECTEX		ConnectEx;
    static LPFN_DISCONNECTEX	DisconnectEx;
    static LPFN_ACCEPTEX		AcceptEx;


    SOCKET CreateSocket()
    {
        return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    }
    void Close(SOCKET& socket)
    {
        if (socket != INVALID_SOCKET)
            ::closesocket(socket);
        socket = INVALID_SOCKET;
    }
    
    //bool Bind(SOCKET socket);
    bool BindAnyAddress(SOCKET socket)
    {
        SOCKADDR_IN myAddress;
        myAddress.sin_family = AF_INET;
        myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
        myAddress.sin_port = ::htons(SERVER_PORT);

        return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
    }

    bool Listen(SOCKET socket, int32 backlog = SOMAXCONN)
    {
        return SOCKET_ERROR != ::listen(socket, backlog);
    }



    template<typename T>
    bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
    {
        return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
    }

    bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
    {
        LINGER option;
        option.l_onoff = onoff;
        option.l_linger = linger;
        return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
    }
    bool SetReuseAddress(SOCKET socket, bool flag)
    {
        return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
    }
    // ListenSocket의 특성을 ClientSocket에 그대로 적용
    bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
    {
        return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
    }

}