#pragma once
#include <array>
#include <mutex>
#include <map>

#include "../Utility/Utility.h"
#include "../Network/Network.h"

class Object;
class Player;
class MapObject;
class Room
{
public:
	Room(int id);
	~Room();

	void Init();
	// Add Object - Player, MapObject, Anything else
	void AddObject(Object* object);
	// Remove Player - If Room want Reset
	void RemovePlayer(Player* player);
	// Disable Player - if Player Connection is Gone, player just disabled not remove
	void DisablePlayer(Player* player);
	//The Game Is End, Reset Room ex)clear mObjects
	void ResetGameRoom();
	
	void ActiveRoom();
	bool IsRoomReady();
	bool IsRoomReadyComplete();
	bool IsAllPlayerReady();
	void PlayerCntIncrease();

	bool SettingRoomPlayer(const sPlayerInfo& playerInfo, const int& playerMaxNum);
	int GetPlayerRoomSyncID(const int uID);
	sPlayerInfo GetPlayerInfo(const int uID);
	
	void PlayerArrive(Player* player);
	//Object order
	//0~7 - player 
	//8~ (MAX_OBJECT-1) - mapobject
	std::array<Object*, MAX_OBJECT>& GetObjectsRef() { return mObjects;}
protected:
	void addPlayer(Player* player);
	void addMapObject(MapObject* mapObject);
	//���� �� ���¸� Free���� ������.

	void RemovePlayerInfo(const int& UID);
	void setPlayerInfo(const sPlayerInfo& playerInfo, const int& playerMaxNum);
	void setPlayerInfoWithCnt(const sPlayerInfo& playerInfo, const int& playerMaxNum, int& playerCnt);

	void setGameEndTimerStartOnce();
protected:
	std::mutex mObjectsLock;
	std::array<Object*,MAX_OBJECT> mObjects;

	std::mutex mPlayerInfoLock;
	std::map<int, sPlayerInfo> mPlayerInfo;

	std::mutex mPlayerArriveLock;
	int	mPlayerArrivedCnt;	//�� ������ ��� ��

	std::mutex mRoomStateLock;
	eRoomState mRoomState;
	int	mRoomID;	//�� ID
	int mPlayerSettingCnt;	//���� �濡 player�� ��� ���� �ƴ���.
	std::atomic_int mPlayerCnt;	//���� �濡 player�� �� �� ���Դ���.
	int mPlayerMax;	//�� �ִ� �ο�
	bool mGameEndTimer;	//The Room Game is Over (Using CAS)

	eRoomStage mRoomStageKindof;	//���߿� ������ �𸣴� ������ �������� ����.

};

