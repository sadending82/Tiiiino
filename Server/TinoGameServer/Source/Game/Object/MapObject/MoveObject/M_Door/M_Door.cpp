#include "M_Door.h"
#include "../../../../Thread/TimerThread/TimerThread.h"

M_Door::M_Door()
{
}

M_Door::~M_Door()
{
}


void M_Door::Init(const int roomID, const int roomSyncID)
{
	__super::Init(roomID, roomSyncID);
	mWaitMilliTime = 5000;	//5 seconds
	TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_DOORSYNC, eEventType::TYPE_BROADCAST_ROOM, mWaitMilliTime, mRoomSyncID, roomID);
}

long long M_Door::MeasureSyncTime(const long long ping)
{
	return __super::MeasureSyncTime(ping);
}

void M_Door::Setting()
{
	__super::Setting();
	mSyncTime += std::chrono::milliseconds(mWaitMilliTime);
}

