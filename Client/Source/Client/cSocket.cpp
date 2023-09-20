#include "cSocket.h"

#include <iostream>
#include <string>

cSocket::cSocket() : threadCounter(0)
{
}

cSocket::~cSocket()
{
    delete thread;
    thread = nullptr;

    closesocket(sock);
    WSACleanup();
}



bool cSocket::InitializeSocket()
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

bool cSocket::Connect()
{
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serveraddr.sin_port = htons(SERVERPORT);
    int ret = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (ret == SOCKET_ERROR) {
        std::cout << "Socket Error : " << WSAGetLastError() << std::endl;
        return false;
    }

    std::cout << "Connection success" << std::endl;

    return true;
}

void cSocket::ReceivePacket()
{

    char buffer[BUFSIZE];
    unsigned int remain_data = 0;

    while (true) {

        DWORD flags = 0;
        int retval = 0;
        DWORD packet_size = 0;

        char recvbuf[BUFSIZE];

        WSABUF r_wsabuf;
        r_wsabuf.buf = recvbuf;
        r_wsabuf.len = BUFSIZE;

        DWORD recvbytes = 0;
        retval = WSARecv(sock, &r_wsabuf, 1, &recvbytes, &flags, NULL, NULL);

        if(retval == SOCKET_ERROR) {
            int err_no = WSAGetLastError();
            if (err_no != WSA_IO_PENDING) {
                closesocket(sock);
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



void cSocket::SetPlayerController(ATinoPlayerController* pPlayerController)
{
    if (pPlayerController)
    {
        m_playerController = pPlayerController;
    }
}

void cSocket::CloseSocket()
{
    closesocket(sock);
    WSACleanup();
}

bool cSocket::Init()
{
    return true;
}

uint32 cSocket::Run()
{
    FPlatformProcess::Sleep(0.03); // 초기 Init 기다리기

    char buffer[BUFSIZE];
    unsigned int remain_data = 0;

    while (threadCounter.GetValue() == 0 && m_playerController != nullptr) {
        DWORD flags = 0;
        int retval = 0;
        DWORD packet_size = 0;

        char recvbuf[BUFSIZE];

        WSABUF r_wsabuf;
        r_wsabuf.buf = recvbuf;
        r_wsabuf.len = BUFSIZE;

        DWORD recvbytes = 0;
        retval = WSARecv(sock, &r_wsabuf, 1, &recvbytes, &flags, NULL, NULL);

        if (retval == SOCKET_ERROR) {
            int err_no = WSAGetLastError();
            if (err_no != WSA_IO_PENDING) {
                closesocket(sock);
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

    return 0;
}

void cSocket::ProcessPacket(char* buf)
{
    switch (buf[1]) {
    case 0:
    default:
        break;
    }
}

void cSocket::Stop()
{
    threadCounter.Increment();
}

void cSocket::Exit()
{
}

bool cSocket::StartListen()
{
    if (thread != nullptr) return false;
    thread = FRunnableThread::Create(this, TEXT("cSocket"), 0, TPri_BelowNormal); //스레드 생성
    //FRunnable로 생성된 녀석은 생성되면 Run을 실행하도록 할 수 있음.
    return (thread != nullptr); // 성공 여부 리턴
}

void cSocket::StopListen()
{
    Stop(); // 이걸 불러주면 thread safe 한 카운터가 1을 증가시켜 주므로, Run에서 실행하던 녀석이 멈춘다.
    thread->WaitForCompletion(); // 끝날때 까지 기다려 주기
    thread->Kill(); // 쓰레드 쥬기기
    delete thread; // 지워줘야 겠죠?

    thread = nullptr;
    threadCounter.Reset(); // 다시 0으로 바꿔주기
}
