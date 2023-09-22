//test ip
constexpr auto SERVERIP = "127.0.0.1";

constexpr int SERVERPORT = 3500;
constexpr int BUF_SIZE = 200;

constexpr int MAX_USER = 300;
constexpr int MAX_OBJECT = 3000;
constexpr int MAX_NAME_SIZE = 20;	//임의값, 기획 통해서 정해주어야함.
constexpr int MAX_ROOM = 10;		//임의값, 기획 통해서 정해주어야함.
constexpr int MAX_ROOM_USER = 8;

enum PacketType {
	// Client To Server
	CS_LOGIN,
	CS_MOVE,
    CS_CHAT,

	// Server To Client
	SC_LOGIN_OK,
	SC_LOGIN_FAIL,
	SC_ADD_PLAYER,
	SC_REMOVE_PLAYER,
	SC_MOVE_PLAYER
};

#pragma pack (push, 1)

struct PACKET {
public:
	unsigned char size;
	char	type;
};

struct CS_LOGIN_PACKET : public PACKET {
	char	name[MAX_NAME_SIZE];
	int		roomID;	//원래는 lobbyServer에서 줘야 하는 값. 나중에 '무조건' 빼야함.
};

struct CS_MOVE_PACKET: public PACKET {
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

struct EVENT {
	unsigned char size;
	char	type;
};

#pragma pack (pop)