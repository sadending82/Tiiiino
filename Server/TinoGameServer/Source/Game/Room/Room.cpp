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
	, mPlayerCnt(0)
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
		if (!p) break;
		if (p->GetSocketID() == player->GetSocketID())
		{
			RemovePlayerInfo(p->GetUID());
			mObjects[i] = nullptr;
			return;
		}
	}
}

void Room::DisablePlayer(Player* player)
{
	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		Player* p = dynamic_cast<Player*>(mObjects[i]);
		if (!p) break;
		if (p->GetSocketID() == player->GetSocketID())
		{
			//player socket state�� disconnect�� ���� �ϸ鼭 free�� �ٲ�.
			RemovePlayerInfo(p->GetUID());
			return;
		}
	}
}


void Room::ResetGameRoom()
{
	for (auto object : mObjects)
	{
		if (!object) continue;
		object->Reset();
		Player* player = dynamic_cast<Player*>(object);
		if (player)
		{
			player->DisConnectAndReset();
		}
		object = nullptr;
	}
	mRoomStageKindof = eRoomStage::ST_AVOID;
	mPlayerInfo.clear();
	mPlayerSettingCnt = 0;
	mPlayerMax = 0;
	mPlayerArrivedCnt = 0;
	mPlayerCnt = 0;
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

bool Room::SettingRoomPlayer(const sPlayerInfo& playerInfo, const int& playerMaxNum)
{
	int playerCnt = -1;
	setPlayerInfoWithCnt(playerInfo, playerMaxNum, playerCnt);
	if (playerCnt == playerMaxNum)
	{
		mRoomStateLock.lock();
		if (mRoomState == eRoomState::ST_READY || mRoomState == eRoomState::ST_FREE)
		{
			DEBUGMSGONEPARAM("�� �غ� �Ϸ�. ���� ��� �ο�[%d]��", mPlayerInfo.size());
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

int Room::GetPlayerRoomSyncID(const int uID)
{
	//�� �Լ��� mPlayerInfo�� �� ������ �� �Ŀ�, �б⸸ �ϴ� �۾��̹Ƿ� lock�� �Ȱɾ����
	//�ִ� �ο��� �ȵ������� ���� ������ ���ɼ��� �ֱ� ������ ���� ������ �ȵ�
	//������ �̷� ��찡 ���� �����س�����, �Ŀ� Ȥ�ø𸣴� ����� �ȵǸ� �ȵǴϱ� assert����.

	//2023-10-05 �߰� -> �ڲ� �����ȵǴ� ������ �Ʒ� assert�� �ɸ�. 
	// �� �Լ��� room�� start�� �Ǳ� ���� ���ʷ� mPlayerMax��ŭ �Ҹ��� �� �̱� ������
	// lock�� �ɾ �ִ� 8�� 1���Ӵ� 8������ ȣ��Ǵ� ���̱� ������ �δ��� ���ٰ� �ǴܵǾ�
	// �̻��� ���׸� �� ����°� �� �ּ��̶� �����Ǿ� ���� �ɱ�� ����.

	mPlayerInfoLock.lock();
	//������ �Ʒ��� iter�� ��ã�����̹Ƿ� ���⼭ �˻����ִ� �ǹ̰� ����. ���� ���⼭ ���װ� �������Ѵٸ�..
	//if (mPlayerInfo.size() != mPlayerMax)
	//{
	//	mPlayerInfoLock.unlock();
	//	assert(0);
	//	return -1;
	//}
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

sPlayerInfo Room::GetPlayerInfo(const int uID)
{
	//�� �Լ��� ���������� �д� �۾��� �ֱ⿡ ���� �Ȱɾ���
	//������ ���⼭�� �ڲ� �������Ͱ� ������ �� �� ������.
	//2023-10-06 ��ȹ�� �����غ��� �������� ����. �׷� ������ playerInfo���� �÷��̾ ���־����.
	//�ְ� ������ �˾Ҵ��� ���߿� �����°͵� �����ϱ� ������ ������ ���� �ɾ����.
	mPlayerInfoLock.lock();
	auto Iter = mPlayerInfo.find(uID);
	if (Iter != mPlayerInfo.end())
	{
		sPlayerInfo tmp = (*Iter).second;
		mPlayerInfoLock.unlock();
		return tmp;
	}
	mPlayerInfoLock.unlock();
	return sPlayerInfo();
}

void Room::PlayerArrive(Player* player)
{
	int tRank = 0;
	mPlayerArriveLock.lock();
	mPlayerArrivedCnt++;
	tRank = mPlayerArrivedCnt;
	mPlayerArriveLock.unlock();
	player->SetRank(tRank);
	setGameEndTimerStartOnce();		//�� �Լ��� room �ϳ��� �� �� �� ����� ���� .lockfree CAS(Compare And Set)���� ����. (mutex �ʹ����̾��°Ű��Ƽ� �������� ��) 

}

void Room::addPlayer(Player* player)
{
	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		//�̺κп��� �����ͷ��̽��� ���� �÷��̾������� ���ļ� ������� ġ���� ������ �߻���.
		//���� lock���� ����.
		//�������� ������ ������, �������� ���� ID�� �����ϱ� ������ �д°� ���� ��������.
		//�÷��̾ �ִ� ������ ���� ������ �� �� �ѹ� Or ƨ���� ������ �� �� �̱� ������ �д°Ŷ� ����.
		//�׸��� �д� ������ SocketState�� �����ϱ� ������, Ȥ�ó� �����ͷ��̽��� �ɸ��ٰ� �ĵ� socketstate���� �ɸ�. �� ������.
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
		//map object���� ���� �������� ���������� �ֱ� ������ �����ͷ��̽� ���� X�׷��� ���� �ɾ���.
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


void Room::RemovePlayerInfo(const int& UID)
{
	mPlayerInfoLock.lock();
	mPlayerInfo.erase(UID);
	mPlayerInfoLock.unlock();
}

void Room::setPlayerInfo(const sPlayerInfo& playerInfo, const int& playerMaxNum)
{
	bool flag = false;
	mPlayerMax = playerMaxNum;
	mPlayerInfoLock.lock();
	if (playerMaxNum == mPlayerSettingCnt)
	{
		mPlayerInfoLock.unlock();
		return;
	}
	mPlayerInfo.insert(std::make_pair(playerInfo.UID, playerInfo));
	mPlayerSettingCnt++;
	mPlayerInfoLock.unlock();

}

void Room::setPlayerInfoWithCnt(const sPlayerInfo& playerInfo, const int& playerMaxNum, int& playerCnt)
{
	bool flag = false;
	mPlayerMax = playerMaxNum;
	mPlayerInfoLock.lock();
	if (playerMaxNum == mPlayerSettingCnt)
	{
		mPlayerInfoLock.unlock();
		DEBUGMSGONEPARAM("�ɰ��� ����!!!! [%d]", mPlayerSettingCnt);
		return;
	}
	mPlayerInfo.insert(std::make_pair(playerInfo.UID, playerInfo));
	mPlayerSettingCnt++;
	playerCnt = mPlayerSettingCnt;
	mPlayerInfoLock.unlock();
}

void Room::setGameEndTimerStartOnce()
{
	bool expect = 0;
	if (std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic_bool*>(&mGameEndTimer), &expect, 1))
	{
		DEBUGMSGNOPARAM("�� �� ����Ǿ���\n");

		TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_COUNTDOWN_START, eEventType::TYPE_BROADCAST_ROOM, 1000, NULL, mRoomID);
	}
}
