#include "Timer.h"

void Timer::Init(HANDLE cHCP)
{
	mHiocp = cHCP;

	//priority_queue<TimerEvent> empty_queue;
	//swap(mTimerQueue, empty_queue);
	//
	//mIsRun = TRUE;
	//
	//thread timer_thread{ (thread(&Timer::TimerMain, this)) };
	//timer_thread.join();
}

void Timer::PushEvent(int ckey, eEVENT_TYPE ceventType, int cdelayStartTime,unsigned char* cmessage)
{
	mTimerLock.lock();
	TimerEvent te;
	te.mKey = ckey;
	te.mEventType = ceventType;
	te.mStartTime = system_clock::now() + milliseconds(cdelayStartTime);
	memcpy(te.mEventMessage, cmessage, cmessage[0]);
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

			OverEXP* over_exp = new OverEXP;
			over_exp->mTargetID = te.mKey;
			over_exp->mCompType = eCompType::OP_EVENT;
			memcpy(over_exp->mMessageBuf, reinterpret_cast<char*>(&te), sizeof(te));

			PostQueuedCompletionStatus(mHiocp, 1, te.mKey, (LPOVERLAPPED)&over_exp->mOver);
		}
		else
		{
			mTimerLock.unlock();
			this_thread::sleep_for(100ms);
		}
	}
}