#pragma once
#include "pch.h"

namespace SocketUtils
{
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

    template<typename T>
    bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
    {
        return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
    }
}