#include <cassert>

#include "Room.h"
#include "../Object/Player/Player.h"
#include "../Object/MapObject/MapObject.h"
#include "../Thread/TimerThread/TimerThread.h"

Room::Room(int id)
	: mRoomStageKindof(eRoomStage::ST_AVOID)
	, mObjects()
	, mPlayerInfo()
	, mPlayerSettingCnt(0)
	, mPlayerCnt(-1)
	, mPlayerMax(0)
	, mRoomState(eRoomState::ST_FREE)
	, mPlayerArrivedCnt(0)
	, mGameEndTimer(false)
	, mRoomID(id)
{

}


Room::~Room()
{
	for (auto object : mObjects)
	{
		delete object;
		object = nullptr;
	}
}

void Room::Init()
{

}

void Room::AddObject(Object* object)
{
	Player* player = dynamic_cast<Player*>(object);
	if (player)
	{
		addPlayer(player);
		return;
	}
	MapObject* mapObject = dynamic_cast<MapObject*>(object);
	if (mapObject)
	{
		addMapObject(mapObject);
		return;
	}
}

void Room::RemovePlayer(Player* player)
{
	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		Player* p = dynamic_cast<Player*>(mObjects[i]);
		if (p->GetSocketID() == player->GetSocketID())
		{
			mObjects[i] = nullptr;
			return;
		}
	}
}


void Room::ResetGameRoom()
{
	for (auto object : mObjects)
	{
		object->Reset();
		Player* player = dynamic_cast<Player*>(object);
		if (player)
		{
			player->DisConnect();
		}
		object = nullptr;
	}
	mRoomStageKindof = eRoomStage::ST_AVOID;
	mPlayerInfo.clear();
	mPlayerSettingCnt = 0;
	mPlayerMax = 0;
	mPlayerArrivedCnt = 0;
	mGameEndTimer = false;

	mRoomStateLock.lock();
	mRoomState = eRoomState::ST_FREE;
	mRoomStateLock.unlock();
}


void Room::ActiveRoom()
{
	mRoomStateLock.lock();
	if (mRoomState == eRoomState::ST_FREE)
	{
		mRoomState = eRoomState::ST_READY;
	}
	mRoomStateLock.unlock();
}

bool Room::IsRoomReadyComplete()
{
	mRoomStateLock.lock();
	if (mRoomState == eRoomState::ST_READY_COMPLETE)
	{
		mRoomStateLock.unlock();
		return true;
	}
	mRoomStateLock.unlock();
	return false;
}

bool Room::IsAllPlayerReady()
{
	if (mPlayerCnt != mPlayerMax) return false;
	mRoomStateLock.lock();
	if (mRoomState == eRoomState::ST_READY_COMPLETE)
	{
		mRoomState = eRoomState::ST_INGAME;
		mRoomStateLock.unlock();
		return true;
	}
	else {
		mRoomStateLock.unlock();
		return false;
	}

}

void Room::PlayerCntIncrease()
{
	mPlayerCnt++;
}

bool Room::IsRoomReady()
{
	mRoomStateLock.lock();
	if (mRoomState == eRoomState::ST_READY)
	{
		mRoomStateLock.unlock();
		return true;
	}
	mRoomStateLock.unlock();
	return false;
}

bool Room::SettingRoomPlayer(const int uID, const std::string id, const int& playerMaxNum)
{
	int playerCnt = -1;
	setPlayerInfoWithCnt(uID, id, playerMaxNum, playerCnt);
	if (playerCnt == playerMaxNum)
	{
		mRoomStateLock.lock();
		if (mRoomState == eRoomState::ST_READY || mRoomState == eRoomState::ST_FREE)
		{
			DEBUGMSGONEPARAM("방 준비 완료. 현재 대기 인원[%d]명", mPlayerInfo.size());
			mRoomState = eRoomState::ST_READY_COMPLETE;
			mRoomStateLock.unlock();
			return true;
		}
		else {
			mRoomStateLock.unlock();
		}
	}
	return false;
}

int Room::FindPlayerInfo(const int uID, const std::string id)
{
	//이 함수는 mPlayerInfo가 다 쓰여진 난 후에, 읽기만 하는 작업이므로 lock을 안걸어놓음
	//최대 인원이 안들어왔으면 아직 쓰여질 가능성이 있기 때문에 절대 읽으면 안됨
	//지금은 이럴 경우가 없게 설계해놨지만, 후에 혹시모르는 설계로 안되면 안되니까 assert걸음.

	//2023-10-05 추가 -> 자꾸 말도안되는 이유로 아래 assert에 걸림. 
	// 이 함수는 room이 start가 되기 직전 최초로 mPlayerMax만큼 불리는 것 이기 떄문에
	// lock을 걸어도 최대 8번 1게임당 8번정도 호출되는 것이기 때문에 부담이 없다고 판단되어
	// 이상한 버그를 안 만드는게 더 최선이라 생각되어 락을 걸기로 결정.

	mPlayerInfoLock.lock();
	if (mPlayerInfo.size() != mPlayerMax)
	{
		mPlayerInfoLock.unlock();
		assert(0);
		return -1;
	}
	auto Iter = mPlayerInfo.find(uID);
	if (Iter != mPlayerInfo.end())
	{
		auto dist = std::distance(mPlayerInfo.begin(), Iter);
		mPlayerInfoLock.unlock();
		return dist;
	}
	mPlayerInfoLock.unlock();
	return -1;
}

void Room::PlayerArrive(Player* player)
{
	int tRank = 0;
	mPlayerArriveLock.lock();
	mPlayerArrivedCnt++;
	tRank = mPlayerArrivedCnt;
	mPlayerArriveLock.unlock();
	player->SetRank(tRank);
	setGameEndTimerStartOnce();		//이 함수는 room 하나당 딱 한 번 실행될 것임 .lockfree CAS(Compare And Set)으로 구현. (mutex 너무많이쓰는거같아서 락프리로 씀) 

}

void Room::addPlayer(Player* player)
{
	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		//이부분에서 데이터레이스가 나면 플레이어정보가 겹쳐서 사라지는 치명적 오류가 발생함.
		//따라서 lock으로 감쌈.
		//넣을때만 문제가 있으며, 읽을때는 각자 ID로 수정하기 때문에 읽는건 전혀 문제없음.
		//플레이어를 넣는 순간은 게임 시작할 때 딱 한번 Or 튕긴사람 재접속 할 때 이기 때문에 읽는거랑 노상관.
		//그리고 읽는 기준은 SocketState로 구분하기 때문에, 혹시나 데이터레이스로 걸린다고 쳐도 socketstate에서 걸림. 즉 안읽힘.
		mObjectsLock.lock();
		if (nullptr == mObjects[i])
		{
			mObjects[i] = player;
			mObjectsLock.unlock();
			player->GetStateLockRef().lock();
			player->SetSocketState(eSocketState::ST_INGAME);
			player->GetStateLockRef().unlock();
			return;
		}
		mObjectsLock.unlock();
	}
}

void Room::addMapObject(MapObject* mapObject)
{
	for (int i = MAX_ROOM_USER; i < MAX_OBJECT; ++i)
	{
		//map object같은 경우는 서버에서 순차적으로 넣기 때문에 데이터레이스 걱정 X그래도 락은 걸어줌.
		mObjectsLock.lock();
		if (nullptr == mObjects[i])
		{
			mObjects[i] = mapObject;
			mObjectsLock.unlock();
			return;
		}
		mObjectsLock.unlock();
	}
}


void Room::setPlayerInfo(const int uID, const std::string id, const int& playerMaxNum)
{
	bool flag = false;
	mPlayerMax = playerMaxNum;
	mPlayerInfoLock.lock();
	if (playerMaxNum == mPlayerSettingCnt)
	{
		mPlayerInfoLock.unlock();
		return;
	}
	mPlayerInfo.insert(std::make_pair(uID, id));
	mPlayerSettingCnt++;
	mPlayerInfoLock.unlock();

}

void Room::setPlayerInfoWithCnt(const int uID, const std::string id, const int& playerMaxNum, int& playerCnt)
{
	bool flag = false;
	mPlayerMax = playerMaxNum;
	mPlayerInfoLock.lock();
	if (playerMaxNum == mPlayerSettingCnt)
	{
		mPlayerInfoLock.unlock();
		DEBUGMSGONEPARAM("심각한 오류!!!! [%d]", mPlayerSettingCnt);
		return;
	}
	mPlayerInfo.insert(std::make_pair(uID, id));
	mPlayerSettingCnt++;
	playerCnt = mPlayerSettingCnt;
	mPlayerInfoLock.unlock();
}

void Room::setGameEndTimerStartOnce()
{
	bool expect = 0;
	if (std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic_bool*>(&mGameEndTimer), 0, 1))
	{
		DEBUGMSGNOPARAM("한 번 실행되야함\n");

		TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_COUNTDOWN_START, eEventType::TYPE_BROADCAST_ROOM, 0, NULL, mRoomID);
	}
}
