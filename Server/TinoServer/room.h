#pragma once
#include "stdafx.h"

enum class eRoomState
{
	RS_FREE, RS_READY, RS_INGAME
};

constexpr double GRADE_FOR_SCORE[5][8] =
{
	{5, 1.7, -1.7, -5, 999, 999, 999, 999},
	{5, 2.5, 0, -2.5, -5, 999, 999, 999},
	{5, 3,1, -1, -3, -5, 999, 999},
	{5, 3.3, 1.7, 0, -1.7, -3.3, -5,999},
	{5, 3.6, 2.1, 0.7, -0.7, -2.1, -3.6, -5}
};

constexpr double GRADE_CON_NUM = 0.01;

constexpr int MIN_USER = 4;

class Room
{
public:
	Room()
	{
		mState = eRoomState::RS_FREE;
		ZeroMemory(mUID, sizeof(mUID));
		ZeroMemory(mSockID, sizeof(mSockID));
		ZeroMemory(mGrade, sizeof(mGrade));
		mUserNum = 0;
		mGradeAvg = 0;
		mUpdateCount = 0;
	}
	~Room() {}

public:
	mutex	mStateLock;
	eRoomState mState;
	int mUID[MAX_ROOM_USER];
	int mSockID[MAX_ROOM_USER];
	double mGrade[MAX_ROOM_USER];
	int mUserNum;
	double mGradeAvg;
	int mUpdateCount;
};