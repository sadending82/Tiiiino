#pragma once
#include "stdafx.h"
using namespace std::chrono;

constexpr int EVENT_BUF_SIZE = 100;

enum class EVENT_TYPE 
{ 
	EV_MATCH_IN, EV_MATCH_OUT
};

struct TimerEvent 
{
	int mKey;
	system_clock::time_point mStartTime;
	EVENT_TYPE mEventType;
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
	void PushEvent(int ckey, EVENT_TYPE ceventType, int cdelayStartTime, char* cmessage);
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