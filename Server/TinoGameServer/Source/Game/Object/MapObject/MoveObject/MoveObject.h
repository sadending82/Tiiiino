#pragma once
#include "../MapObject.h"

class MoveObject : public MapObject
{
public:
	MoveObject();
	virtual ~MoveObject();

	virtual void Init(const int roomID, const int roomSyncID);
	virtual void Setting();
	virtual long long MeasureSyncTime(const long long ping);

	long long GetWaitMilliTime() const { return mWaitMilliTime; }

protected:
	std::chrono::milliseconds mSyncTime;
	long long mWaitMilliTime;
	int			mRoomID;		//Room Number
	int			mRoomSyncID;	//Room Sync Number in Client, use Objects[mRoomSyncID]
};

