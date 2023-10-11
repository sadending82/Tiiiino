#pragma once
#include "timer.h"

void Timer::Init(HANDLE cHCP)
{
	mHiocp = cHCP;

	priority_queue<TimerEvent> empty_queue;
	swap(mTimerQueue, empty_queue);

	mIsRun = TRUE;
}

void Timer::PushEvent(int ckey, eEVENT_TYPE ceventType, int cdelayStartTime)
{
	mTimerLock.lock();
	TimerEvent te;
	te.mKey = ckey;
	te.mEventType = ceventType;
	te.mStartTime = system_clock::now() + milliseconds(cdelayStartTime);
	mTimerQueue.push(te);
	mTimerLock.unlock();
}

void Timer::TimerMain()
{
	while (mIsRun)
	{
		mTimerLock.lock();
		if ((mTimerQueue.empty() == FALSE) && (mTimerQueue.top().mStartTime <= system_clock::now()))
		{
			TimerEvent te = mTimerQueue.top();
			mTimerQueue.pop();
			mTimerLock.unlock();

			OverlappedEx* over = new OverlappedEx;

			PostQueuedCompletionStatus(mHiocp, 1, te.mKey, (LPOVERLAPPED)&over->over);
		}
		else
		{
			mTimerLock.unlock();
			this_thread::sleep_for(100ms);
		}
	}
}