#pragma once
#include "server.h"

void Server::Disconnect(int cID)
{
	mClients[cID].mStateLock.lock();
	if (mClients[cID].mState == eSessionState::ST_FREE) {
		mClients[cID].mStateLock.unlock();
		return;
	}
	closesocket(mClients[cID].mSocket);
	mClients[cID].mState = eSessionState::ST_FREE;
	mClients[cID].mStateLock.unlock();
}

int Server::GetNewClientID()
{
	for (int i = 0; i < MAX_USER; ++i) {
		mClients[i].mStateLock.lock();
		if (mClients[i].mState == eSessionState::ST_FREE) {
			mClients[i].mState = eSessionState::ST_ACCEPTED;
			mClients[i].mStateLock.unlock();
			return i;
		}
		mClients[i].mStateLock.unlock();
	}
	return INVALID_KEY;
}

void Server::ProcessPacket(int cID, char* cpacket)
{
	switch (cpacket[1]) 
	{
	case NONE:
	{
		break;
	}
	default:
	{
		break;
	}
	}
}

void Server::DoWorker()
{
	while (true)
	{
		DWORD numBytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(mHCP, &numBytes, &key, &over, INFINITE);
		OverEXP* exOver = reinterpret_cast<OverEXP*>(over);
		int client_id = static_cast<int>(key);
		if (FALSE == ret)
		{
			if (exOver->mCompType == eCompType::OP_ACCEPT)
			{
				cout << "Accept Error";
			}
			else
			{
				cout << "GQCS Error on client[" << key << "]\n";
				Disconnect(static_cast<int>(key));
				if (exOver->mCompType == eCompType::OP_SEND)
				{
					delete exOver;
				}
				continue;
			}
		}

		switch (exOver->mCompType) 
		{
		case eCompType::OP_ACCEPT:
		{
			SOCKET cSocket = reinterpret_cast<SOCKET>(exOver->mWsaBuf.buf);
			int client_id = GetNewClientID();
			if (client_id != INVALID_KEY)
			{
				mClients[client_id].mPlayerID = client_id;
				mClients[client_id].mPrevRemain = 0;
				mClients[client_id].mSocket = cSocket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(cSocket), mHCP, client_id, 0);
				mClients[client_id].DoRecv();
				cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				cout << "player connect\n";
			}
			else
			{
				cout << "Max user exceeded.\n";
			}

			ZeroMemory(&exOver->mOver, sizeof(exOver->mOver));
			exOver->mWsaBuf.buf = reinterpret_cast<CHAR*>(cSocket);
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(mListenSocket, cSocket, exOver->mMessageBuf, 0, addr_size + 16, addr_size + 16, 0, &exOver->mOver);
			break;
		}
		case eCompType::OP_RECV:
		{
			if (0 == numBytes)
			{
				Disconnect(client_id);
			}
			int remainData = numBytes + mClients[key].mPrevRemain;
			char* p = exOver->mMessageBuf;
			while (remainData > 0)
			{
				int packetSize = p[0];
				if (packetSize <= remainData)
				{
					ProcessPacket(static_cast<int>(key), p);
					p = p + packetSize;
					remainData = remainData - packetSize;
				}
				else
				{
					break;
				}
			}
			mClients[key].mPrevRemain = remainData;
			if (remainData > 0)
			{
				memcpy(exOver->mMessageBuf, p, remainData);
			}
			mClients[key].DoRecv();
			break;
		}
		case eCompType::OP_SEND:
		{
			if (0 == numBytes)
			{
				Disconnect(client_id);
			}
			delete exOver;
			break;
		}
		case eCompType::OP_EVENT: 
		{
			ProcessEvent(exOver->mMessageBuf);
			break;
		}
		default :
		{
			break;
		}
		}
	}
}

void Server::Init()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVERPORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(mListenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	listen(mListenSocket, SOMAXCONN);

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	int client_id = 0;

	mHCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(mListenSocket), mHCP, 9999, 0);

	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	OverEXP a_over;
	a_over.mCompType = eCompType::OP_ACCEPT;
	a_over.mWsaBuf.buf = reinterpret_cast<CHAR*>(c_socket);
	AcceptEx(mListenSocket, c_socket, a_over.mMessageBuf, 0, addr_size + 16, addr_size + 16, 0, &a_over.mOver);

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i)
		mWorkerThreads.emplace_back(thread(&Server::DoWorker, this));

	for (auto& th : mWorkerThreads)
		th.join();

	closesocket(mListenSocket);
	WSACleanup();
}

void Server::ProcessEvent(char* cmessage)
{
	switch (cmessage[1]) {
	case NONE:
	{
		break;
	}
	default: 
	{
		break;
	}
	}
}

HANDLE Server::GetHandle()
{
	return mHCP;
}
