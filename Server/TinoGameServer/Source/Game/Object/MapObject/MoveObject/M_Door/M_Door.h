#pragma once
#include "../MoveObject.h"

class M_Door : public MoveObject
{
public:
	M_Door();
	virtual ~M_Door();

	void Init(const int roomID,const int roomSyncID);
	long long MeasureSyncTime(const long long ping);

	long long GetWaitTime() const { return mWaitTime; }
protected:
	std::chrono::milliseconds mSyncTime;
	long long mWaitTime;
	int			mRoomID;		//Room Number
	int			mRoomSyncID;	//Room Sync Number in Client, use Objects[mRoomSyncID]
};

