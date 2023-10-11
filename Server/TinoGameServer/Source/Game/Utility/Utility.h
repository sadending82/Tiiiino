#pragma once
#include <iostream>
#include <chrono>
#include "../../../../../protocol.h"
constexpr int INVALID_SOCKET_ID = -1;
constexpr int INVALID_ROOM_ID = -1;
constexpr int ROOM_RESET_TIME = 30000;

#ifdef _DEBUG
#define DEBUGMSGNOPARAM(args) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args))
#define DEBUGMSGONEPARAM(args,k) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args,k))
#else
#define DEBUGMSGNOPARAM(...)
#define DEBUGMSGONEPARAM(...)
#endif


enum class eEventType : char
{
	TYPE_BROADCAST_ALL,		//전체 기반
	TYPE_BROADCAST_ROOM,	//ROOM 기반
	TYPE_SELF_EXCEPT,		//ROOM 기반
	TYPE_SELF,				//ROOM 기반
	TYPE_TARGET,
};

enum class eCOMMAND_IOCP {
	CMD_NONE,CMD_ACCEPT, CMD_RECV, CMD_SEND, CMD_SERVER_RECV, CMD_DBSERVER_RECV, //Basic
	CMD_GAME_WAIT, CMD_GAME_START, CMD_GAME_END, CMD_GAME_RESET, CMD_GAME_COUNTDOWN_START, //Game Cycle
	CMD_PING,
	CMD_GAME_DOORSYNC, CMD_GAME_BREAKDOOR, CMD_GAME_BREAKPLATFORM	//Game Object
};

//SocketState
enum class eSocketState
{
	ST_FREE,
	ST_ACCEPT,
	ST_INGAME
};


//Stage Name - 스테이지 이름 넣어야함.
enum class eRoomStage
{
	ST_RUN,
	ST_AVOID
};

enum class eRoomState
{
	ST_FREE,			//빈 방 empty room
	ST_READY,			//사람을 받고 있는 방 people come into room 
	ST_READY_COMPLETE,	//사람을 다 받은 방		people come into room complete
	ST_INGAME,			//게임중인 방
	ST_CLOSED,			//곧 리셋이 될 방.
};


//Player Have Item BitFlag with int64
enum class eEquipmentFlags : __int64
{
	None = 0,
};

enum class ePlayerState
{
	ST_RUNNING,
	ST_ARRIVED,
};

struct Vector3i {
	int x, y, z;
	Vector3i() = default;
	Vector3i(int x, int y, int z) : x(x), y(y), z(z) {};
};

struct Vector3f {
	float x, y, z;
	Vector3f() = default;
	Vector3f(float x, float y, float z) : x(x), y(y), z(z) {};
};

struct Vector4f {
	float x, y, z, w;
	Vector4f() = default;
	Vector4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
};

struct TimerEvent {

	int senderID{};
	int receiverID{};
	Vector3i pos{};
	std::chrono::system_clock::time_point execTime{};
	eCOMMAND_IOCP type{};
	eEventType eventType{};

	constexpr bool operator < (const TimerEvent& R) const
	{
		return (execTime > R.execTime);
	}
};

struct sPlayerInfo {
	std::string ID;
	std::string NickName;
	eDepartment Department;
	eEquipmentFlags Equipment;
	int RoomID;
	int UID = -1;
	char	hashs[MAX_NAME_SIZE];
};