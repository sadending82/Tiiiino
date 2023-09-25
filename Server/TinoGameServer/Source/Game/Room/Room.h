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
	bool IsRoomActive();

	bool SettingRoomPlayer(const int uID, const std::string id, const int& playerMaxNum);
	bool FindPlayerInfo(const int uID, const std::string id);
	
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
protected:
	std::mutex mObjectsLock;
	std::array<Object*,MAX_OBJECT> mObjects;

	std::mutex mPlayerInfoLock;
	std::map<int, std::string> mPlayerInfo;

	std::mutex mRoomStateLock;
	eRoomState mRoomState;
	int mPlayerCnt;	//현재 방에 player가 몇명 들어왔는지.
	int mPlayerMax;	//방 최대 인원

	eRoomStage mRoomStageKindof;	//나중에 생길지 모르는 종류별 스테이지 대비용.
};

