#include "MoveObject.h"

MoveObject::MoveObject()
	:mSyncTime()
	, mWaitMilliTime()
	, mRoomID()
	,mRoomSyncID()
{	
}

MoveObject::~MoveObject()
{
}

void MoveObject::Init(const int roomID, const int roomSyncID)
{
	mRoomID = roomID;
	mRoomSyncID = roomSyncID;
	Setting();
}

long long MoveObject::MeasureSyncTime(const long long ping)
{
    return mSyncTime.count() - (ping>>1);	// 핑은 양방향을 쟀을 때 기준 이기 때문에, 나누기 2해야 됨. round trip time(rtt)
}

void MoveObject::Setting()
{
	mSyncTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::high_resolution_clock::now().time_since_epoch());
}
