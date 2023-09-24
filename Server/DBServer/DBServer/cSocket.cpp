#pragma once
#include "cSocket.h"

#define Test

int Socket::SetKey()
{
    int cnt = STARTKEY;
    while (true) {
        if (cnt == MAXLOBBY)
            return INVALIDKEY;
        if (eSessionState::ST_FREE == mSessions[cnt].mState) {
            return cnt;
        }
        else
            ++cnt;
    }
}

void Socket::Disconnect(int key) 
{
    mSessions[key].mStateLock.lock();
    if (mSessions[key].mState == eSessionState::ST_FREE) {
        mSessions[key].mStateLock.unlock();
        return;
    }
    closesocket(mSessions[key].mSocket);
    mSessions[key].mState = eSessionState::ST_FREE;
    mSessions[key].mStateLock.unlock();
}

void Socket::WorkerFunc()
{
    DWORD Transferred;
    ULONG key;
    WSAOVERLAPPED* over;

    while (1) {
        BOOL retval = GetQueuedCompletionStatus(mHcp, &Transferred,
            (PULONG_PTR)&key, &over, INFINITE);

        ExOver* exOver = reinterpret_cast<ExOver*>(over);

        if (false == retval) {
            Disconnect(key);
        }

        switch (exOver->mOpType) {
        case eOpType::OP_ACCEPT: {
            int newKey = SetKey();
            if (newKey != INVALIDKEY) {
                mSessions[newKey].mSocket = exOver->mSocket;
                mSessions[newKey].mExOver.mOpType = eOpType::OP_RECV;
                mSessions[newKey].mPrevData= 0;
                mSessions[newKey].mState = eSessionState::ST_ACCEPTED;
                mSessions[newKey].mID = key;
                CreateIoCompletionPort((HANDLE)mSessions[key].mSocket, mHcp, key, 0);
                mSessions[newKey].DoRecv();
            }
            else {
                Disconnect(newKey);
                break;
            }

            ZeroMemory(&exOver->mOver, sizeof(exOver->mOver));
            SOCKET cSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            exOver->mSocket = cSocket;
            BOOL ret = AcceptEx(mListenSocket, cSocket
                , exOver->mMessageBuf
                , 0, 32, 32, NULL, &exOver->mOver);
            if (false == ret) {
                int err_num = WSAGetLastError();
                if (err_num != WSA_IO_PENDING)
                {
#ifdef Test
                    std::cout << "Accept Error: " << err_num << std::endl;
#endif
                }
            }
            break;
        }
        case eOpType::OP_RECV: {
            unsigned char* packet_ptr = exOver->mMessageBuf;
            int required_data = Transferred + mSessions[key].mPrevData;
            int packet_size = packet_ptr[0];
            while (required_data >= packet_size) {
                if (required_data >= BUFSIZE) break;
                if (packet_size <= 0) break;
                processPacket(key, packet_ptr);
                required_data -= packet_size;
                packet_ptr += packet_size;
                packet_size = packet_ptr[0];
            }
            packet_size = 0;
            mSessions[key].mPrevData= 0;
            if (0 != required_data)
                memcpy(exOver->mMessageBuf, packet_ptr, required_data);
            mSessions[key].DoRecv();
            break;
        }
        case eOpType::OP_SEND: {
            delete exOver;
            break;
        }
        }

    }
}

void Socket::ServerReady(DB* pDB)
{
    Setm_pDB(pDB);

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	mHcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(mListenSocket), mHcp, 0, 0);

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(DBSERVERPORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(mListenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	listen(mListenSocket, SOMAXCONN);

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	int client_id = 0;

    ExOver over;
    over.mSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	over.SetmOpType(eOpType::OP_ACCEPT);
	bool ret = AcceptEx(mListenSocket, over.mSocket, over.mMessageBuf, 0, addr_size + 16, addr_size + 16, 0, &over.mOver);

	if (false == ret) {
		int err_num = WSAGetLastError();
		if (err_num != WSA_IO_PENDING)
		{
#ifdef Test
			std::cout << "Accept Error: " << err_num << std::endl;
#endif
            return;
		}
	}
#ifdef Test
    std::cout << "Connect To Lobby Server Ready" << std::endl;
#endif

    WorkerFunc();
}

void Socket::processPacket(int key, unsigned char* buf)
{
    switch (buf[1])
    {
    case 0:
    {
        break;
    }
    default:
    {
        break;
    }
    }
}
