#pragma once
#include "stdafx.h"

enum class eSessionState
{ 
	ST_FREE, ST_ACCEPTED, ST_LOBBY, ST_MATCH, ST_INGAME
};

class Session
{
public:
	Session()
	{
		mSocketID = -1;
		mSocket = 0;
		mState = eSessionState::ST_FREE;
		mPrevRemain = 0;
<<<<<<< Updated upstream
=======
		mRoomID = -1;
		mUID = 0;
		ZeroMemory(mNickName, sizeof(mNickName));
		mCredit = 0;
		mPoint = 0;
		mTier = 0;
		mMatchStartTime = system_clock::now();
>>>>>>> Stashed changes
	}
	~Session() {}
	void DoRecv()
	{
		DWORD recvFlag = 0;
		memset(&mRecvOver.mOver, 0, sizeof(mRecvOver.mOver));
		mRecvOver.mWsaBuf.len = BUF_SIZE - mPrevRemain;
		mRecvOver.mWsaBuf.buf = mRecvOver.mMessageBuf + mPrevRemain;
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
<<<<<<< Updated upstream
	int mPlayerID;
	SOCKET mSocket;
=======
	int		mSocketID;
	int		mUID;
	char	mID[MAX_NAME_SIZE];
	char	mNickName[MAX_NAME_SIZE];
	double	mCredit;
	int		mPoint;
	double	mTier;
	system_clock::time_point mMatchStartTime;
	SOCKET	mSocket;
>>>>>>> Stashed changes
	int		mPrevRemain;
};