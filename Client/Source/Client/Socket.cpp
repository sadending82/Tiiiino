#include "Socket.h"
#include <iostream>

bool Socket::Init()
{
    int retval = 0;

    WSADATA wsa;
    int nRet = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (nRet != 0) {
        std::cout << "Socket Error : " << WSAGetLastError() << std::endl;
        return false;
    }

    sock = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket Error : " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

bool Socket::Connect()
{
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(SERVERPORT);
    int ret = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (ret == SOCKET_ERROR) {
        std::cout << "Socket Error : " << WSAGetLastError() << std::endl;
        return false;
    }

    std::cout << "Connection success" << std::endl;

    return true;
}
