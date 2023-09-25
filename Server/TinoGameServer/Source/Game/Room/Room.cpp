#include <cassert>

#include "Room.h"
#include "../Object/Player/Player.h"
#include "../Object/MapObject/MapObject.h"

Room::Room()
	: mRoomStageKindof(eRoomStage::ST_AVOID)
	,mObjects()
	, mPlayerInfo()
	, mPlayerCnt(0)
	, mPlayerMax(0)
	, mRoomState(eRoomState::ST_FREE)
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
		object = nullptr;
	}
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

bool Room::IsRoomActive()
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
	setPlayerInfoWithCnt(uID,id, playerMaxNum, playerCnt);
	if (playerCnt == playerMaxNum)
	{
		mRoomStateLock.lock();
		if (mRoomState == eRoomState::ST_READY || mRoomState == eRoomState::ST_FREE)
		{
			mRoomState = eRoomState::ST_READY_COMPLETE;
		}
		mRoomStateLock.unlock();
		return true;
	}
	return false;
}

bool Room::FindPlayerInfo(const int uID, const std::string id)
{
	//이 함수는 mPlayerInfo가 다 쓰여진 난 후에, 읽기만 하는 작업이므로 lock을 안걸어놓음
	//최대 인원이 안들어왔으면 아직 쓰여질 가능성이 있기 때문에 절대 읽으면 안됨
	//지금은 이럴 경우가 없게 설계해놨지만, 후에 혹시모르는 설계로 안되면 안되니까 assert걸음.
	if (mPlayerInfo.size() != mPlayerMax)
		assert(0);
	if (id == mPlayerInfo.at(uID))
	{
		return true;
	}

	return false;
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
		//map object같은 경우는 서버에서 순차적으로 넣기 때문에 데이터레이스 걱정 X
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
	if (playerMaxNum == mPlayerCnt)
	{
		mPlayerInfoLock.unlock();
		return;
	}
	mPlayerInfo.insert(std::make_pair(uID,id));
	mPlayerCnt++;
	mPlayerInfoLock.unlock();

}

void Room::setPlayerInfoWithCnt(const int uID, const std::string id, const int& playerMaxNum, int& playerCnt)
{
	bool flag = false;
	mPlayerMax = playerMaxNum;
	mPlayerInfoLock.lock();
	if (playerMaxNum == mPlayerCnt)
	{
		mPlayerInfoLock.unlock();
		return;
	}
	mPlayerInfo.insert(std::make_pair(uID,id));
	mPlayerCnt++;
	playerCnt = mPlayerCnt;
	mPlayerInfoLock.unlock();
}
