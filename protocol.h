//test ip
<<<<<<< Updated upstream
constexpr auto SERVERIP = "127.0.0.1";
=======
#pragma once
constexpr const char* SERVERIP = "127.0.0.1";
constexpr const char* LOOBYSERVER_ADDR = "127.0.0.1";
>>>>>>> Stashed changes

constexpr int SERVERPORT = 3500;
constexpr int BUF_SIZE = 200;

constexpr int MAX_USER = 300;

enum PacketType {
	// Client To Server
	CS_LOGIN,
	CS_MOVE,
	CS_STOP,
	CS_ROTATE,
    CS_CHAT,
	CS_GOAL,
	CS_PING,
	CS_ACTION,

<<<<<<< Updated upstream
	// Server To Client
=======
	// Client To LobbyServer
	CL_LOGIN,
	CL_SIGNUP,
	CL_MATCH,
	CL_MATCH_OUT,

	// GameServer To Client
>>>>>>> Stashed changes
	SC_LOGIN_OK,
	SC_LOGIN_FAIL,
	SC_ADD_PLAYER,
	SC_PLAYER_REMOVE,
	SC_MOVE_PLAYER,
<<<<<<< Updated upstream
	SC_STOP_PLAYER,
	SC_ROTATE_PLAYER,
=======
	SC_PLAYER_ARRIVE,
	SC_GAME_END,
	SC_GAME_COUNTDOWN_START,
	SC_PING,
	SC_ACTION_ANIM,
	SC_GAME_DOORSYNC,

	// Lobbyserver To Client
	LC_LOGIN_OK,
	LC_LOGIN_FAIL,
	LC_MATCH_RESPONSE,
>>>>>>> Stashed changes
};

#pragma pack (push, 1)

struct PACKET {
public:
	unsigned char size;
	char	type;
};

<<<<<<< Updated upstream
struct CS_MOVE_PACKET: public PACKET {
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned  client_time;
};

struct CS_STOP_PACKET : public PACKET {
	unsigned  client_time;
};

struct CS_ROTATE_PACKET : public PACKET {

	unsigned  client_time;
=======
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
>>>>>>> Stashed changes
};

struct CS_CHAT_PACKET : public PACKET {

	char mess[BUF_SIZE];
};

<<<<<<< Updated upstream
=======
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

struct CL_SIGNUP_PACKET :public PACKET {
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
>>>>>>> Stashed changes
struct SC_MOVE_PLAYER_PACKET : public PACKET {
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned  client_time;
};

struct SC_STOP_PLAYER_PACKET : public PACKET {
	unsigned  client_time;
};

<<<<<<< Updated upstream
struct SC_ROTATE_PLAYER_PACKET : public PACKET {

	unsigned  client_time;
};

=======
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
	//char	record;		//성공 실패
};

struct SC_PING_PACKET : public PACKET {
	long long	ping;
};

struct SC_ACTION_ANIM_PACKET : public PACKET {
	// 0 : none, 1 : jump, 2 : dive 
	char action;		
	int id;
};

struct SC_GAME_DOORSYNC_PACKET : public PACKET {
	long long	syncTime;
	int			id;
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
>>>>>>> Stashed changes
struct EVENT {
	unsigned char size;
	char	type;
};

#pragma pack (pop)