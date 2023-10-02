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
	Room();
	~Room();

	void Init();
	// Add Object - Player, MapObject, Anything else
	void AddObject(Object* object);
	// Remove Player - If Player Connection is Gone
	void RemovePlayer(Player* player);
	//The Game Is End, Reset Room ex)clear mObjects
	void ResetGameRoom();
	
	void ActiveRoom();
	bool IsRoomReady();
	bool IsRoomReadyComplete();

	bool SettingRoomPlayer(const int uID, const std::string id, const int& playerMaxNum);
	bool FindPlayerInfo(const int uID, const std::string id);
	
	void PlayerArrive(Player* player);
	//Object order
	//0~7 - player 
	//8~ (MAX_OBJECT-1) - mapobject
	std::array<Object*, MAX_OBJECT>& GetObjectsRef() { return mObjects;}
protected:
	void addPlayer(Player* player);
	void addMapObject(MapObject* mapObject);
	//게임 룸 상태를 Free에서 깨워줌.

	void setPlayerInfo(const int uID, const std::string id, const int& playerMaxNum);
	void setPlayerInfoWithCnt(const int uID,const std::string id, const int& playerMaxNum, int& playerCnt);

	void setGameEndTimerStartOnce();
protected:
	std::mutex mObjectsLock;
	std::array<Object*,MAX_OBJECT> mObjects;

	std::mutex mPlayerInfoLock;
	std::map<int, std::string> mPlayerInfo;

	std::mutex mPlayerArriveLock;
	int	mPlayerArrivedCnt;	//골에 도착한 사람 수

	std::mutex mRoomStateLock;
	eRoomState mRoomState;
	int mPlayerCnt;	//현재 방에 player가 몇명 들어왔는지.
	int mPlayerMax;	//방 최대 인원
	bool mGameEndTimer;

	eRoomStage mRoomStageKindof;	//나중에 생길지 모르는 종류별 스테이지 대비용.

};

