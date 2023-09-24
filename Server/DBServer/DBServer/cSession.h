#pragma once
#include "stdafx.h"
#include "cExOver.h"

enum class eSessionState
{
	ST_FREE, ST_ACCEPTED
};

class Session
{
public:
	Session()
	{
		mID = -1;
		mSocket = 0;
		mState = eSessionState::ST_FREE;
		mPrevData= 0;
	}
	~Session() {}
	void DoRecv()
	{
		DWORD recvFlag = 0;
		ZeroMemory(&mExOver.mOver, sizeof(mExOver.mOver));
		mExOver.mWsaBuf.len = BUF_SIZE - mPrevData;
		mExOver.mWsaBuf.buf = reinterpret_cast<char*>(mExOver.mMessageBuf) + mPrevData;
		WSARecv(mSocket, &mExOver.mWsaBuf, 1, 0, &recvFlag, &mExOver.mOver, 0);
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) {
			cout << "Recv Error " <<" code: " << err_no << endl;
		}
	}
	void DoSend(void* packet)
	{
		ExOver* sdata = new ExOver{ reinterpret_cast<char*>(packet) };
		WSASend(mSocket, &sdata->mWsaBuf, 1, 0, 0, &sdata->mOver, 0);
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) {
			cout << "Send Error " << " code: " << err_no << endl;
		}
	}

public:
	ExOver mExOver;
	mutex	mStateLock;
	eSessionState mState;
	int mID;
	SOCKET mSocket;
	int	mPrevData;
};