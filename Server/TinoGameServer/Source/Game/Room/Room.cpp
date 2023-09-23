#include "Room.h"
#include "../Object/Player/Player.h"
#include "../Object/MapObject/MapObject.h"

Room::Room()
	: mRoomStageKindof(eRoomStage::ST_AVOID)
	,mObjects()
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
		AddPlayer(player);
		return;
	}
	MapObject* mapObject = dynamic_cast<MapObject*>(object);
	if (mapObject)
	{
		AddMapObject(mapObject);
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

void Room::AddPlayer(Player* player)
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

void Room::AddMapObject(MapObject* mapObject)
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
