#include "Socket.h"
#include <iostream>

bool Socket::Init()
{
    int retval = 0;

    WSADATA wsa;
    int nRet = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (nRet != 0) {
        //std::cout << "Socket Error : " << WSAGetLastError() << std::endl;
        return false;
    }

    m_sock = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_sock == INVALID_SOCKET) {
        //std::cout << "Socket Error : " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

bool Socket::Connect()
{
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    int ret = connect(m_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (ret == SOCKET_ERROR) {
        //std::cout << "Socket Error : " << WSAGetLastError() << std::endl;
        return false;
    }

    //std::cout << "Connection success" << std::endl;

    return true;
}

void Socket::ReceivePacket()
{

    char buffer[BUF_SIZE];
    unsigned int remain_data = 0;

    while (true) {

        DWORD flags = 0;
        int retval = 0;
        DWORD packet_size = 0;

        char recvbuf[BUF_SIZE];

        WSABUF r_wsabuf;
        r_wsabuf.buf = recvbuf;
        r_wsabuf.len = BUF_SIZE;

        DWORD recvbytes = 0;
        retval = WSARecv(m_sock, &r_wsabuf, 1, &recvbytes, &flags, NULL, NULL);

        if(retval == SOCKET_ERROR) {
            int err_no = WSAGetLastError();
            if (err_no != WSA_IO_PENDING) {
                closesocket(m_sock);
                break;
            }
        }

        unsigned int rest_size = recvbytes;
        unsigned char* buf_ptr = reinterpret_cast<unsigned char*>(recvbuf);

        while (rest_size > 0)
        {
            if (0 == packet_size)
                packet_size = recvbytes;
            if (rest_size + remain_data >= packet_size) {
                std::memcpy(buffer + remain_data, buf_ptr, packet_size - remain_data);
                ProcessPacket(buffer);
                buf_ptr += packet_size - remain_data;
                rest_size -= packet_size - remain_data;
                packet_size = 0;
                remain_data = 0;
            }
            else {
                std::memcpy(buffer + remain_data, buf_ptr, rest_size);
                remain_data += rest_size;
                rest_size = 0;
            }
        }
    }

}

bool Socket::SendPacket(char* buf)
{
    DWORD sendbytes = 0;
    WSABUF wsabuf;

    wsabuf.len = buf[0];
    wsabuf.buf = buf;

    int retval = WSASend(m_sock, &wsabuf, 1, &sendbytes, 0, NULL, NULL);
    if (retval == SOCKET_ERROR) {
        return false;
        //std::cout << "Send Packet Error: %d: " << WSAGetLastError() << std::endl;
    }
    return true;
}

void Socket::ProcessPacket(char* buf)
{
    switch (buf[1]) {
    case 0:
    default:
        break;
    }
}
