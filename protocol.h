//test ip
#pragma once
constexpr const char* SERVERIP = "127.0.0.1";
constexpr const char* LOOBYSERVER_ADDR = "127.0.0.1";

constexpr int GAMESERVERPORT = 3600;
constexpr int LOBBYSERVERPORT = 3500;
constexpr int BUF_SIZE = 200;

constexpr int MAX_USER = 300;
constexpr int MAX_OBJECT = 3000;
constexpr int MAX_NAME_SIZE = 20;	//닉네임, 중복검사 X
constexpr int MAX_ROOM = 10;
constexpr int MAX_ROOM_USER = 8;

enum PacketType {
	// Client To GameServer
	CS_LOGIN,
	CS_MOVE,
    CS_CHAT,
	CS_GOAL,
	CS_PING,
	CS_ACTION,

	// Client To LobbyServer
	CL_LOGIN,
	CL_JOIN,
	CL_MATCH,
	CL_MATCH_OUT,

	// GameServer To Client
	SC_LOGIN_OK,
	SC_LOGIN_FAIL,
	SC_ADD_PLAYER,
	SC_PLAYER_REMOVE,
	SC_MOVE_PLAYER,
	SC_PLAYER_ARRIVE,
	SC_GAME_END,
	SC_GAME_COUNTDOWN_START,
	SC_PING,
	SC_ACTION_ANIM,

	// Lobbyserver To Client
	LC_LOGIN_OK,
	LC_LOGIN_FAIL,
	LC_MATCH_RESPONSE,
};

#pragma pack (push, 1)

struct PACKET {
public:
	unsigned char size;
	char	type;
};

struct CS_LOGIN_PACKET : public PACKET {
	char	name[MAX_NAME_SIZE];
	char	passWord[MAX_NAME_SIZE];
	int		uID;
	int		roomID;	//원래는 lobbyServer에서 줘야 하는 값. 나중에 '무조건' 빼야함.

};

struct CS_MOVE_PACKET : public PACKET {
	bool inair;					//in air? (for anim)
	float x, y, z;				//pos
	float rx, ry, rz, rw;		//rotate
	float sx, sy, sz;			//Speed For Interpolation
	float speed;				//scala
	int		move_time;		//ping
};

struct CS_CHAT_PACKET : public PACKET {

	char mess[BUF_SIZE];
};

struct CS_GOAL_PACKET : public PACKET {

};

struct CS_PING_PACKET : public PACKET {
	int		ping;
};

struct CS_ACTION_PACKET : public PACKET {
	char action;		// 0 : none, 1 : jump, 2 : dive 
};
//-----------------------------------
struct CL_LOGIN_PACKET :public PACKET {
	char id[MAX_NAME_SIZE];
	char password[MAX_NAME_SIZE];
};

struct CL_JOIN_PACKET :public PACKET {
	char id[MAX_NAME_SIZE];
	char password[MAX_NAME_SIZE];
};

struct CL_MATCH_PACKET : public PACKET {

};

struct CL_MATCH_OUT_PACKET : public PACKET {

};
//-----------------------------------

struct SC_LOGIN_OK_PACKET : public PACKET {
	int		id;
};
struct SC_MOVE_PLAYER_PACKET : public PACKET {
	int		id;
	float x, y, z;
	float rx, ry, rz, rw;
	float speed;
	float sx, sy, sz;		//speed for interpolation
	int		move_time;		//ping
};

struct SC_ADD_PLAYER_PACKET : public PACKET {
	int id;
	float x, y, z;
	float rx, ry, rz, rw;
	char	name[MAX_NAME_SIZE];
};

struct SC_PLAYER_REMOVE_PACKET : public PACKET {
	int id;
};
struct SC_PLAYER_ARRIVE_PACKET : public PACKET
{
	int id;				//who arrived?
};

struct SC_GAME_COUNTDOWN_START_PACKET : public PACKET
{

};

struct SC_GAME_END_PACKET : public PACKET {
	char	record;		//성공 실패
};

struct SC_PING_PACKET : public PACKET {
	long long	ping;
};

struct SC_ACTION_ANIM_PACKET : public PACKET {
	// 0 : none, 1 : jump, 2 : dive 
	char action;		
	int id;
};
//---------------------------
struct LC_LOGIN_OK_PACKET : public PACKET {
	int id;
	int UID;
	int RoomID;
};

struct 	LC_LOGIN_FAIL_PACKET :public PACKET {

};

struct LC_MATCH_RESPONSE_PACKET : public PACKET {
	int gameServerPortNum;
	unsigned char gameServerIP[16];	//IPv4에서 ip는 4바이트인데 글자로 표기하니까 최대 xxx.xxx.xxx.xxx  15 + 널문자 = 16
};

//--------------------------
struct EVENT {
	unsigned char size;
	char	type;
};

#pragma pack (pop)