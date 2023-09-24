#pragma once
#include "stdafx.h"

enum class eSessionState
{ 
	ST_FREE, ST_ACCEPTED, ST_MATCH
};

class Session
{
public:
	Session()
	{
		mPlayerID = -1;
		mSocket = 0;
		mState = eSessionState::ST_FREE;
		mPrevRemain = 0;
	}
	~Session() {}
	void DoRecv()
	{
		DWORD recvFlag = 0;
		memset(&mRecvOver.mOver, 0, sizeof(mRecvOver.mOver));
		mRecvOver.mWsaBuf.len = BUF_SIZE - mPrevRemain;
		mRecvOver.mWsaBuf.buf = reinterpret_cast<char*>(mRecvOver.mMessageBuf + mPrevRemain);
		WSARecv(mSocket, &mRecvOver.mWsaBuf, 1, 0, &recvFlag, &mRecvOver.mOver, 0);
	}
	void DoSend(void* packet)
	{
		OverEXP* sdata = new OverEXP{ reinterpret_cast<char*>(packet) };
		WSASend(mSocket, &sdata->mWsaBuf, 1, 0, 0, &sdata->mOver, 0);
	}

public:
	OverEXP mRecvOver;
	mutex	mStateLock;
	eSessionState mState;
	int mPlayerID;
	SOCKET mSocket;
	int		mPrevRemain;
};