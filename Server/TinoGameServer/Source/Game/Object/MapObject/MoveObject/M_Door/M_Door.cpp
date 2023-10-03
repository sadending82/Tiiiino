#include "M_Door.h"
#include "../../../../Thread/TimerThread/TimerThread.h"

M_Door::M_Door()
	:mSyncTime()
	, mWaitTime(5000)
{
}

M_Door::~M_Door()
{
}


void M_Door::Init(const int roomID, const int roomSyncID)
{
	mRoomID = roomID;
	mRoomSyncID = roomSyncID;
	TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_DOORSYNC, eEventType::TYPE_BROADCAST_ROOM, mWaitTime, mRoomSyncID, roomID);
}

long long M_Door::MeasureSyncTime(const long long ping)
{
	return mSyncTime.count() - ping;
}

