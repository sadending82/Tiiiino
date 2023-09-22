#pragma once
#include <array>
#include <mutex>

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

	//Object order
	//0~7 - player 
	//8~ (MAX_OBJECT-1) - mapobject
	std::array<Object*, MAX_OBJECT>& GetObjectsRef() { return mObjects; }
protected:
	void AddPlayer(Player* player);
	void AddMapObject(MapObject* mapObject);

protected:
	std::mutex mObjectsLock;
	std::array<Object*,MAX_OBJECT> mObjects;

	eRoomStage mRoomStageKindof;	//나중에 생길지 모르는 종류별 스테이지 대비용.
};

