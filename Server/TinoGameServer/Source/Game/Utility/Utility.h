#pragma once

constexpr int INVALID_SOCKET_ID = -1;
constexpr int INVALID_ROOM_ID = -1;

enum class eCOMMAND_IOCP {
	CMD_ACCEPT, CMD_RECV, CMD_SEND, CMD_SERVER_RECV, CMD_DBSERVER_RECV, //Basic
	CMD_GAME_WAIT, CMD_GAME_START, CMD_GAME_END, CMD_GAME_RESET //Game Cycle
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
	ST_FREE,
	ST_READY,
	ST_READY_COMPLETE,
	ST_INGAME
};
//Player Have Item BitFlag with int64
enum class eItemFlags : __int64
{
	None = 0
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
