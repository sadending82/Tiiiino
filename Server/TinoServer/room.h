#pragma once
#include "stdafx.h"

enum class eRoomState
{
	RS_FREE, RS_READY, RS_INGAME
};

class Room
{
public:
	Room()
	{
		mState = eRoomState::RS_FREE;
		ZeroMemory(UID, sizeof(UID));
	}
	~Room() {}

public:
	mutex	mStateLock;
	eRoomState mState;
	int UID[MAX_ROOM_USER];
};