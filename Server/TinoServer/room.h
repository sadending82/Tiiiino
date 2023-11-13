#pragma once
#include "stdafx.h"

enum class eRoomState
{
	RS_FREE, RS_READY, RS_INGAME
};

constexpr double GRADE_FOR_SCORE[5][8]	=
{
	{5,		3.3,	1.6,	0,		999,	999,	999,	999},
	{5,		3.7,	2.5,	1.2,	0,		999,	999,	999},
	{5,		4,		3,		2,		1,		0,		999,	999},
	{5,		4.1,	3.3,	2.5,	1.6,	0.8,	0,		999},
	{5,		4.2,	3.5,	2.8,	2.1,	1.4,	0.7,	0}
};

constexpr int POINT_FOR_SCORE[5][8]	=
{
	{210,	170,	130,	90,		999,	999,	999,	999},
	{240,	200,	160,	120,	80,		999,	999,	999},
	{270,	230,	190,	150,	110,	70,		999,	999},
	{300,	260,	220,	180,	140,	100,	60,		999},
	{330,	290,	250,	210,	170,	130,	90,		50}
};

constexpr double GRADE_CON_NUM = 0.005;

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
		ZeroMemory(mPoint, sizeof(mPoint));
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
	int	mRoomLevel;
	int mPoint[MAX_ROOM_USER];
};