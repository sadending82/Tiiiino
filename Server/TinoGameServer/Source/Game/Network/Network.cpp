#include <iostream>

#include "Network.h"

using namespace std;

WSA_OVER_EX::WSA_OVER_EX(eCOMMAND_IOCP cmd,unsigned char bytes, void* msg)
	: mCmd(cmd)
{
	ZeroMemory(&mWsaOver, sizeof(mWsaOver));
	mWsaBuf.buf = reinterpret_cast<char*>(mBuf);
	mWsaBuf.len = bytes;
	memcpy(mBuf, msg, bytes);
}

WSA_OVER_EX::~WSA_OVER_EX()
{
}




Network::Network()
	:mSocket(INVALID_SOCKET)
	, mSocketState(eSocketState::ST_FREE)
	, mPrevSize(0)
	, mSocketID(INVALID_SOCKET_ID)
{
}

Network::~Network()
{

}

void Network::DisConnect()
{
	mStateLock.lock();
	closesocket(mSocket);
	mSocket = INVALID_SOCKET;
	mSocketState = eSocketState::ST_FREE;
	mStateLock.unlock();
	
#ifdef _DEBUG
	cout << "Connection DisConnect\n";
#endif
	/*
	do SomeThing;
	*/
}

void Network::ErrorDisplay(int errNo)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, 0);
	std::wcout << errNo <<","<<lpMsgBuf << std::endl;
	//while (true);
	LocalFree(lpMsgBuf);
}

void Network::PreRecvPacket(unsigned char* remainMsg, int remainBytes)
{
	mPrevSize = 0;
	if (remainBytes > 0)
	{
		mPrevSize = remainBytes;
		memcpy(mWsaOverEx.GetBuf(), remainMsg, remainBytes);
	}

	ZeroMemory(&mWsaOverEx.GetWsaOver(), sizeof(mWsaOverEx.GetWsaOver()));
	mWsaOverEx.GetWsaBuf().buf = reinterpret_cast<char*>(mWsaOverEx.GetBuf() + mPrevSize);
	mWsaOverEx.GetWsaBuf().len = BUF_SIZE - mPrevSize;
}

bool Network::RecvPacket()
{
	DWORD flags = 0;
	int ret = WSARecv(mSocket, &mWsaOverEx.GetWsaBuf(), 1, 0, &flags, &mWsaOverEx.GetWsaOver(), NULL);
	if (SOCKET_ERROR == ret) {
		int err = WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			ErrorDisplay(err);
			return false;
			DisConnect();
		}
	}
	return true;
}


bool Network::SendPacket(void* packet, int bytes)
{
	WSA_OVER_EX* wsa_ex = new WSA_OVER_EX(eCOMMAND_IOCP::CMD_SEND, bytes, packet);
	int ret = WSASend(mSocket, &wsa_ex->GetWsaBuf(), 1, 0, 0, &wsa_ex->GetWsaOver(), NULL);
	if (SOCKET_ERROR == ret) {
		int err = WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			ErrorDisplay(err);
			return false;
			DisConnect();
		}
	}
	return true;
}

void Network::AcceptSetting(const eSocketState& socketState, const eCOMMAND_IOCP& commandIocp, SOCKET& socket)
{
	mSocketState = socketState;
	mWsaOverEx.SetCmd(commandIocp);
	mSocket = socket;
}

