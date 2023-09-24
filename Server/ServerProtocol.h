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

	// LobbyServer To DBServer
	LD_LOGIN
};


#pragma pack (push, 1)

struct SPACKET {
public:
	unsigned char size;
	char	type;
};

struct GL_LOGIN_PACKET : public SPACKET {
	char	name[MAX_NAME_SIZE];
	int		roomID;	//원래는 lobbyServer에서 줘야 하는 값. 나중에 '무조건' 빼야함.
};

struct LD_LOGIN_PACKET :public SPACKET {
	int		testNum;
};

#pragma pack (pop)