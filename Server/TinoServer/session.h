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
		mRoomID = -1;
		mUID = 0;
		ZeroMemory(mNickName, sizeof(mNickName));
		mGrade = 0;
		mPoint = 0;
		mMatchStartTime = system_clock::now();
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
		OverEXP* sdata = new OverEXP{ reinterpret_cast<unsigned char*>(packet) };
		WSASend(mSocket, &sdata->mWsaBuf, 1, 0, 0, &sdata->mOver, 0);
	}
	void ServerDoSend(void* packet)
	{
		ServerOverEXP* sdata = new ServerOverEXP{ reinterpret_cast<unsigned char*>(packet) };
		WSASend(mSocket, &sdata->mWsaBuf, 1, 0, 0, &sdata->mOver, 0);

	}
public:
	OverEXP mRecvOver;
	mutex	mStateLock;
	eSessionState mState;
	int		mSocketID;
	int		mUID;
	char	mID[MAX_NAME_SIZE];
	char	mNickName[MAX_NAME_SIZE];
	double	mGrade;
	int		mPoint;
	system_clock::time_point mMatchStartTime;
	SOCKET	mSocket;
	int		mPrevRemain;
	int		mDepartment;
	int		mRoomID;	//수민이 임시로 추가해놓음. 얘가 몇번 방에 있는지 확인하기 위함.
	char		mHashs[MAX_NAME_SIZE];	//수민이 추가함. 이 hash값을 사용하여 겜서버에서 클라 검증을 할 예정.
};