#include <cassert>

#include "Room.h"
#include "../Object/Player/Player.h"
#include "../Object/MapObject/MapObject.h"
#include "../Thread/TimerThread/TimerThread.h"
#include "../Server/MainServer/MainServer.h"

Room::Room(int id)
	: mRoomStageKindof(eRoomStage::ST_AVOID)
	, mObjects()
	, mPlayerInfo()
	, mPlayerSettingCnt(0)
	, mPlayerCnt(0)
	, mPlayerMax(0)
	, mRoomState(eRoomState::ST_FREE)
	, mPlayerArrivedCnt(0)
	, mGameEndFlag(false)
	, mGameEndTimer(false)
	, mGameStartTimer(false)
	, mRoomID(id)
{

}


Room::~Room()
{
	for (auto& object : mObjects)
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
		if (!p) continue;
		if (p->GetSocketID() == player->GetSocketID())
		{
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
		if (!p) continue;
		if (p->GetSocketID() == player->GetSocketID())
		{
			//player socket state는 disconnect을 먼저 하면서 free로 바뀜.
			RemovePlayerInfo(p->GetUID());
			return;
		}
	}
}


void Room::ResetGameRoom()
{

	DEBUGMSGONEPARAM("방 리셋 락 전 [%d]\n", mRoomID);
	mRoomStateLock.lock();
	if (mRoomState == eRoomState::ST_CLOSED)
	{
		mRoomState = eRoomState::ST_FREE;
		mRoomStateLock.unlock();
	}
	else {
		mRoomStateLock.unlock();
		return;
	}
	DEBUGMSGONEPARAM("방 리셋 불림 [%d]\n", mRoomID);
	for (auto& object : mObjects)
	{
		if (!object) continue;
		Player* player = dynamic_cast<Player*>(object);
		if (player)
		{
			player->DisConnectAndResetUseInRoom();
		}
		else {
			object->Reset();
		}
		object = nullptr;
	}
	DEBUGMSGONEPARAM("애들 다 내보냄 [%d]\n", mRoomID);
	mRoomStageKindof = eRoomStage::ST_AVOID;
	mPlayerInfo.clear();
	mPlayerSettingCnt = 0;
	mPlayerMax = 0;
	mPlayerArrivedCnt = 0;
	mPlayerCnt = 0;
	mGameEndFlag = false;
	mGameEndTimer = false;
	mGameStartTimer = false;


	gMainServer->send_room_reset_packet(mRoomID);
	DEBUGMSGONEPARAM("패킷까지 다 보냄 [%d]\n", mRoomID);
}

std::mutex& Room::GetRoomStateLockRef()
{
	return mRoomStateLock;
	// TODO: 여기에 return 문을 삽입합니다.
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
	mRoomReadyLock.lock();
	if (mPlayerCnt < mPlayerMax) {
		mRoomReadyLock.unlock();
		return false;
	}
	mRoomReadyLock.unlock();

	DEBUGMSGONEPARAM("방 시작[%d]\n", mRoomID);
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

void Room::SetRoomEnd()
{
	mRoomStateLock.lock();
	if (mRoomState == eRoomState::ST_INGAME)
	{
		mRoomState = eRoomState::ST_CLOSED;	//곧 리셋 될 방.
		mRoomStateLock.unlock();
		return;
	}
	else {
		mRoomStateLock.unlock();
		return;
	}

}


void Room::PlayerCntIncrease()
{
	mPlayerCnt++;
}

void Room::PlayerMaxDecrease()
{
	mPlayerMax--;
	if (mPlayerMax <= 0)
	{
		DEBUGMSGONEPARAM("방 에누군가 나감 방이 끝남.[%d]\n", mRoomID);
		TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_END, eEventType::TYPE_TARGET, 0, 10, mRoomID);
	}
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
	printf("방에 들어오는중. 현재 인원[%d]명, 방넘버[%d], 넘어오는패킷 [%d]\n", playerCnt, mRoomID, playerMaxNum);
	if (playerCnt == playerMaxNum)
	{
		mRoomStateLock.lock();
		if (mRoomState == eRoomState::ST_READY || mRoomState == eRoomState::ST_FREE)
		{
			DEBUGMSGONEPARAM("방 준비 완료. 현재 대기 인원[%d]명\n", mPlayerInfo.size());
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
	//이 함수는 mPlayerInfo가 다 쓰여진 난 후에, 읽기만 하는 작업이므로 lock을 안걸어놓음
	//최대 인원이 안들어왔으면 아직 쓰여질 가능성이 있기 때문에 절대 읽으면 안됨
	//지금은 이럴 경우가 없게 설계해놨지만, 후에 혹시모르는 설계로 안되면 안되니까 assert걸음.

	//2023-10-05 추가 -> 자꾸 말도안되는 이유로 아래 assert에 걸림. 
	// 이 함수는 room이 start가 되기 직전 최초로 mPlayerMax만큼 불리는 것 이기 떄문에
	// lock을 걸어도 최대 8번 1게임당 8번정도 호출되는 것이기 때문에 부담이 없다고 판단되어
	// 이상한 버그를 안 만드는게 더 최선이라 생각되어 락을 걸기로 결정.

	mPlayerInfoLock.lock();
	//어차피 아래서 iter가 못찾을것이므로 여기서 검사해주는 의미가 없음. 물론 여기서 버그가 터지긴한다만..
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
	//윗 함수와 마찬가지로 읽는 작업만 있기에 락을 안걸었음
	//하지만 여기서도 자꾸 널포인터가 나오면 락 걸 생각임.
	//2023-10-06 기획을 생각해보니 재접속이 없음. 그럼 나가면 playerInfo에서 플레이어를 빼주어야함.
	//넣고 끝인줄 알았더니 도중에 나가는것도 존재하기 때문에 무조건 락을 걸어양함.
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
	setGameEndTimerStartOnce();		//이 함수는 room 하나당 딱 한 번 실행될 것임 .lockfree CAS(Compare And Set)으로 구현. (mutex 너무많이쓰는거같아서 락프리로 씀) 
	if (tRank == mPlayerMax)
	{
		//누가 고의적으로 이 함수가 실행 될 때, 접속을 끊으면 이 곳에 안들어 올 수도 있음
		// 근데 어차피 20초뒤에 방이 끝나니 크리티컬하지 않으므로 예외처리 하지 않음.
		//If someone intentionally disconnects when this line is executed, they may not be able to enter here.
		// But since the room ends after 20 seconds anyway, it is not critical, so no exception is handled.
		AllPlayerArrived();
	}
}

void Room::AllPlayerArrived()
{
	DEBUGMSGONEPARAM("설마 AllPlayerArrive에서? [%d]\n", mRoomID);
	TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_END, eEventType::TYPE_BROADCAST_ROOM, 0, NULL, mRoomID);
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
		DEBUGMSGONEPARAM("심각한 오류!!!! [%d]", mPlayerSettingCnt);
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
		DEBUGMSGNOPARAM("게임 엔드 타이머 한 번 실행되야함\n");

		TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_COUNTDOWN_START, eEventType::TYPE_BROADCAST_ROOM, 0, NULL, mRoomID);
	}
}

bool Room::IsGameEndOnce()
{
	bool expect = 0;
	if (std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic_bool*>(&mGameEndFlag), &expect, 1))
	{
		DEBUGMSGONEPARAM("게임 끝 한 번 실행되야함 [%d]\n", mRoomID);
		return true;
	}
	return false;
}

void Room::setGameStartTimerStartOnce()
{
	bool expect = 0;
	if (std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic_bool*>(&mGameStartTimer), &expect, 1))
	{
		DEBUGMSGNOPARAM("게임 스타트 타이머 한 번 실행되야함\n");

		TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_WAIT, eEventType::TYPE_BROADCAST_ROOM, 1000, NULL, mRoomID);
	}
}
