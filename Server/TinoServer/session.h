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
		ZeroMemory(mID, sizeof(mID));
		mGrade = 0;
		mPoint = 0;
		mEquippedItems = 0;
		mInventory = 0;
		mMatchStartTime = system_clock::now();
	}
	~Session() {}

	void Reset()
	{
		mSocketID = -1;
		mSocket = 0;
		mState = eSessionState::ST_FREE;
		mPrevRemain = 0;
		mRoomID = -1;
		mUID = 0;
		ZeroMemory(mNickName, sizeof(mNickName));
		ZeroMemory(mID, sizeof(mID));
		mGrade = 0;
		mPoint = 0;
		mEquippedItems = 0;
		mInventory = 0;
		mMatchStartTime = system_clock::now();
	}

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
	long long mEquippedItems;
	long long mInventory;
	int		mRoomID;	//������ �ӽ÷� �߰��س���. �갡 ��� �濡 �ִ��� Ȯ���ϱ� ����.
	char		mHashs[MAX_NAME_SIZE];	//������ �߰���. �� hash���� ����Ͽ� �׼������� Ŭ�� ������ �� ����.
};