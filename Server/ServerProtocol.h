#pragma once


constexpr int DBSERVERPORT = 3700;

constexpr int MAXGAMESERVER = 1;


constexpr unsigned long long INCODE_SERVER_PACKET = 0xFFFF'5137'0808'6310;



enum SPacketType {
	// LobbyServer To GameServer
	LG_LOGIN_OK,
	LG_MOVE,
	LG_CHAT,

	// GameServer To LobbyServer
	GL_LOGIN,
	GL_ROOM_READY,

	// LobbyServer To GameServer
	LG_USER_INTO_GAME,

	// LobbyServer To DBServer
	LD_LOGIN,

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

//---
struct LG_USER_INTO_GAME_PACKET : public SPACKET {
	char	name[MAX_NAME_SIZE];
	int		uID;
	int		roomID;
	int		roomMax;	//방 최대 인원. (4명이서 할 수도 있으니까)
};

struct LD_LOGIN_PACKET :public SPACKET {
	char	id[MAX_NAME_SIZE];
	char	password[MAX_NAME_SIZE];
};

struct DL_LOGIN_OK_PACKET :public SPACKET {
	int		uid;
	char	nickname[MAX_NAME_SIZE];
	double	credit;
	int		point;
};

struct DL_LOGIN_FAIL_PACKET :public SPACKET {
};

#pragma pack (pop)