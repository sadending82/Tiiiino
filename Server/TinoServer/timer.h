#pragma once
#include "stdafx.h"
using namespace std::chrono;

constexpr int EVENT_BUF_SIZE = 100;

struct TimerEvent 
{
	int mKey;
	system_clock::time_point mStartTime;
	eEVENT_TYPE mEventType;
	char mEventMessage[EVENT_BUF_SIZE + 1];

	constexpr bool operator< (const TimerEvent& other) const
	{
		return mStartTime > other.mStartTime;
	}
};

class Timer
{
public:
	priority_queue<TimerEvent> mTimerQueue;
	void Init(HANDLE cHCP);
	void PushEvent(int ckey, eEVENT_TYPE ceventType, int cdelayStartTime, char* cmessage);
	void TimerMain();
	void SetIsRun(bool cvalue) 
	{
		mIsRun = cvalue; 
	}

private:
	int mIsRun;
	HANDLE mHiocp;
	mutex mTimerLock;
};