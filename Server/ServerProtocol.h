#pragma once


constexpr int DBSERVERPORT = 3700;

constexpr int MAXGAMESERVER = 2;


constexpr unsigned long long INCODE_SERVER_PACKET = 0xFFFF'5137'0808'6310;



enum SPacketType {
	// LobbyServer To GameServer
	LG_LOGIN_OK,
	LG_MOVE,
	LG_CHAT,

	// GameServer To LobbyServer
	GL_LOGIN,
	GL_ROOM_READY,
	GL_ROOM_RESET,

	// LobbyServer To GameServer
	LG_USER_INTO_GAME,
	LG_USER_DISCONNECT,

	// LobbyServer To DBServer
	LD_LOGIN,
	LD_SIGNUP,

	// DBServer To LobbyServer
	DL_LOGIN_OK,
	DL_LOGIN_FAIL
};


#pragma pack (push, 1)

struct SPACKET {
public:
	unsigned char size;
	char	type;
};

struct GL_LOGIN_PACKET : public SPACKET {

};

struct GL_ROOM_READY_PACKET : public SPACKET {
	int		roomID;
};

struct GL_ROOM_RESET_PACKET : public SPACKET {
	int		roomID;
};
//---
struct LG_LOGIN_OK_PACKET : public SPACKET {

};
struct LG_USER_INTO_GAME_PACKET : public SPACKET {
	char	id[MAX_NAME_SIZE];	//player id Not UID
	char	name[MAX_NAME_SIZE];
	int		uID;
	int		roomID;
	int		roomMax;	//�� �ִ� �ο�. (4���̼� �� ���� �����ϱ�)
};

struct LG_USER_DISCONNECT_PACKET : public SPACKET {
	int		uID;
	int		roomID;
};

struct LD_LOGIN_PACKET :public SPACKET {
	char	id[MAX_NAME_SIZE];
	char	password[MAX_NAME_SIZE];
	int		userKey;
};

struct LD_SIGNUP_PACKET :public SPACKET {
	char	id[MAX_NAME_SIZE];
	char	password[MAX_NAME_SIZE];
};

struct DL_LOGIN_OK_PACKET :public SPACKET {
	int		uid;
	char	id[MAX_NAME_SIZE];
	char	nickname[MAX_NAME_SIZE];
	double	grade;
	int		point;
	int		userKey;
	double	tier;
	bool	connState;
};

struct DL_LOGIN_FAIL_PACKET :public SPACKET {
	int		userKey;
};

#pragma pack (pop)