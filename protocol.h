//test ip
constexpr auto SERVERIP = "127.0.0.1";

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

	// Server To Client
	SC_LOGIN_OK,
	SC_LOGIN_FAIL,
	SC_ADD_PLAYER,
	SC_REMOVE_PLAYER,
	SC_MOVE_PLAYER,
	SC_STOP_PLAYER,
	SC_ROTATE_PLAYER,
};

enum EventType {
	NONE
};

#pragma pack (push, 1)

struct PACKET {
public:
	unsigned char size;
	char	type;
};

struct CS_MOVE_PACKET: public PACKET {
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned  client_time;
};

struct CS_STOP_PACKET : public PACKET {
	unsigned  client_time;
};

struct CS_ROTATE_PACKET : public PACKET {

	unsigned  client_time;
};

struct CS_CHAT_PACKET : public PACKET {

	char mess[BUF_SIZE];
};

struct SC_MOVE_PLAYER_PACKET : public PACKET {
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned  client_time;
};

struct SC_STOP_PLAYER_PACKET : public PACKET {
	unsigned  client_time;
};

struct SC_ROTATE_PLAYER_PACKET : public PACKET {

	unsigned  client_time;
};

struct EVENT {
	unsigned char size;
	char	type;
};

#pragma pack (pop)