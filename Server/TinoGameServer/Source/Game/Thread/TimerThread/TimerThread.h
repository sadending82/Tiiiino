#pragma once
#include <concurrent_priority_queue.h>
#include "../TinoThread.h"
#include "../../Utility/Utility.h"

extern concurrency::concurrent_priority_queue<TimerEvent> gTimerQueue;
class TimerThread : public TinoThread
{
public:
	TimerThread();
	virtual ~TimerThread();


	virtual void doThread() override;

	//if you don't use param, just push NULL
	static void MakeTimerEventMilliSec(const eCOMMAND_IOCP timerType, const eEventType eventType, const int execTimeMilli, int senderID, int receiverID);
	static void MakeTimerEventMilliSec(const eCOMMAND_IOCP timerType, const eEventType eventType, const int execTimeMilli, int senderID, int receiverID, const Vector3i& pos);

	static eEventType DeserializeEventType(unsigned char* buf);
	static int DeserializeReceiver(unsigned char* buf);
	static Vector3i DeserializePos(unsigned char* buf);
};

