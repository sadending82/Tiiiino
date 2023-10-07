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
		BOOL ret = WSARecv(mSocket, &mExOver.mWsaBuf, 1, 0, &recvFlag, &mExOver.mOver, 0);
	}
	void DoSend(void* packet)
	{
		ExOver* sdata = new ExOver{ reinterpret_cast<char*>(packet) };
		BOOL ret = WSASend(mSocket, &sdata->mWsaBuf, 1, 0, 0, &sdata->mOver, 0);
	}

	ExOver GetExOver() { return mExOver; }
	void SetExOver(ExOver over) { mExOver = over; }

	eSessionState GetState() { return mState; }
	void SetState(eSessionState state) { mState = state; }

	int GetID() { return mID; }
	void SetID(int id) { mID = id; }

	SOCKET GetSocket() { return mSocket; }
	void SetSocket(SOCKET sock) { mSocket = sock; }

	int GetPrevData() { return mPrevData; }
	void SetPrevData(int prevData) { mPrevData = prevData; }

private:
	ExOver mExOver;
	mutex	mStateLock;
	eSessionState mState;
	int mID;
	SOCKET mSocket;
	int	mPrevData;
};