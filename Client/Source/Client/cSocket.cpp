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
    FPlatformProcess::Sleep(0.03); // �ʱ� Init ��ٸ���

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
    thread = FRunnableThread::Create(this, TEXT("cSocket"), 0, TPri_BelowNormal); //������ ����
    //FRunnable�� ������ �༮�� �����Ǹ� Run�� �����ϵ��� �� �� ����.
    return (thread != nullptr); // ���� ���� ����
}

void cSocket::StopListen()
{
    Stop(); // �̰� �ҷ��ָ� thread safe �� ī���Ͱ� 1�� �������� �ֹǷ�, Run���� �����ϴ� �༮�� �����.
    thread->WaitForCompletion(); // ������ ���� ��ٷ� �ֱ�
    thread->Kill(); // ������ ����
    delete thread; // ������� ����?

    thread = nullptr;
    threadCounter.Reset(); // �ٽ� 0���� �ٲ��ֱ�
}
