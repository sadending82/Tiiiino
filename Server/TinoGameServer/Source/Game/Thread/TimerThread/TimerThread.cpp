#include <chrono>
#include "TimerThread.h"
#include "../../Server/MainServer/MainServer.h"

using namespace std;

concurrency::concurrent_priority_queue<TimerEvent> gTimerQueue;

TimerThread::TimerThread()
{
}

TimerThread::~TimerThread()
{
}

void TimerThread::doThread()
{
	bool triger = false;
	TimerEvent isAlready;
	while (true) {
		//뽑고 다음번에 돌아갈 것 넣어줌 .
		while (true) {
			if (triger)
			{
				if (isAlready.execTime <= chrono::system_clock::now())
				{
					WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
					wsa_ex->SetCmd(isAlready.type);
					memcpy(wsa_ex->GetBuf(), &isAlready.eventType, sizeof(char));
					memcpy(wsa_ex->GetBuf() + sizeof(char), &isAlready.receiverID, sizeof(int));
					memcpy(wsa_ex->GetBuf() + sizeof(char) + sizeof(int), &isAlready.pos, sizeof(Vector3i));
					PostQueuedCompletionStatus(gMainServer->GetIOCPHandle(), 1, isAlready.senderID, &wsa_ex->GetWsaOver());
					triger = false;

				}
				continue;
			}
			TimerEvent execEvent;
			if (!gTimerQueue.try_pop(execEvent))
				break;
			if (execEvent.execTime <= chrono::system_clock::now())
			{
				WSA_OVER_EX* wsa_ex = new WSA_OVER_EX;
				wsa_ex->SetCmd(isAlready.type);
				memcpy(wsa_ex->GetBuf(), &isAlready.eventType, sizeof(char));
				memcpy(wsa_ex->GetBuf() + sizeof(char), &isAlready.receiverID, sizeof(int));
				memcpy(wsa_ex->GetBuf() + sizeof(char) + sizeof(int), &isAlready.pos, sizeof(Vector3i));
				PostQueuedCompletionStatus(gMainServer->GetIOCPHandle(), 1, isAlready.senderID, &wsa_ex->GetWsaOver());
			}
			else {

				if (execEvent.execTime > chrono::system_clock::now() + 100ms)
				{// 기다려야 하는 시간이 0.1초이상이라면 다른 더 중요한게 이 사이에 들어올 수 있으므로. 0.1초정도만 기다려줌. 그리고 다시 넣음.
					//원래 1초였는데... 두리안 공격시간을 0.1초로 수정해주기 위해 이것도 자연스럽게... 줄어들음 ,,,
					gTimerQueue.push(execEvent);
					this_thread::sleep_for(100ms);
				}
				else {
					isAlready = execEvent;
					triger = true;
					break;
				}

			}
		}
		this_thread::sleep_for(chrono::duration_cast<chrono::milliseconds>(isAlready.execTime - chrono::system_clock::now()));
	}
}

void TimerThread::MakeTimerEventMilliSec(const eCOMMAND_IOCP timerType, const eEventType eventType, const int execTimeMilli, int senderID, int receiverID)
{
	TimerEvent instq;
	instq.type = timerType;
	instq.eventType = eventType;
	instq.senderID = senderID;
	instq.receiverID = receiverID;
	instq.execTime = chrono::system_clock::now() + chrono::milliseconds(execTimeMilli);
	gTimerQueue.push(instq);
}

void TimerThread::MakeTimerEventMilliSec(const eCOMMAND_IOCP timerType, const eEventType eventType, const int execTimeMilli, int senderID, int receiverID, const Vector3i& pos)
{
	TimerEvent instq;
	instq.type = timerType;
	instq.eventType = eventType;
	instq.senderID = senderID;
	instq.receiverID = receiverID;
	instq.pos = pos;
	instq.execTime = chrono::system_clock::now() + chrono::milliseconds(execTimeMilli);
	gTimerQueue.push(instq);
}

eEventType TimerThread::DeserializeEventType(unsigned char* buf)
{
	eEventType eventType = static_cast<eEventType>(buf[0]);
	return eventType;



	switch (eventType)
	{
	case eEventType::TYPE_BROADCAST_ALL:
		break;
	case eEventType::TYPE_BROADCAST_ROOM:
		break;
	case eEventType::TYPE_SELF_EXCEPT:
		break;
	case eEventType::TYPE_SELF:
		break;
	case eEventType::TYPE_TARGET:
		break;
	}

}

int TimerThread::DeserializeReceiver(unsigned char* buf)
{
	int receiverID;
	memcpy(&receiverID, &buf[sizeof(char)], sizeof(int));
	return receiverID;
}

Vector3i TimerThread::DeserializePos(unsigned char* buf)
{
	Vector3i pos;
	memcpy(&pos, &buf[sizeof(char) + sizeof(int)], sizeof(Vector3i));
	return pos;

}





