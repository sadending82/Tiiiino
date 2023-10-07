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
	GL_PLAYER_RESULT,

	// LobbyServer To GameServer
	LG_USER_INTO_GAME,
	LG_USER_DISCONNECT,

	// LobbyServer To DBServer
	LD_LOGIN,
	LD_LOGOUT,
	LD_SIGNUP,
	LD_UPDATE_NICKNAME,
	LD_UPDATE_GRADE,
	LD_CHANGE_DEPARTMENT,

	// DBServer To LobbyServer
	DL_LOGIN_OK,
	DL_LOGIN_FAIL,
	DL_UPDATE_NICKNAME_OK,
	DL_CHANGE_DEPARTMENT_OK,
	DL_SIGNUP_OK,
	DL_SIGNUP_FAIL
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

struct GL_PLAYER_RESULT_PACKET : public SPACKET {
	int		uID;
	int		rank;
	int		RoomID;
	bool	retire;
};
//---
struct LG_LOGIN_OK_PACKET : public SPACKET {

};
struct LG_USER_INTO_GAME_PACKET : public SPACKET {
	char	id[MAX_NAME_SIZE];	//player id Not UID
	char	name[MAX_NAME_SIZE];
	char	department;
	char	hashs[MAX_NAME_SIZE];	//verification value
	int		uID;
	int		roomID;
	int		roomMax;	// in room Max User
	long long	equipmentflag;

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

struct LD_LOGOUT_PACKET :public SPACKET {
	int		uid;
};

struct LD_SIGNUP_PACKET :public SPACKET {
	char	id[MAX_NAME_SIZE];
	char	password[MAX_NAME_SIZE];
	int		userKey;
};

struct LD_UPDATE_NICKNAME_PACKET :public SPACKET {
	char	nickname[MAX_NAME_SIZE];
	int		uid;
	int		userKey;
};

struct LD_UPDATE_GRADE_PACKET :public SPACKET {
	int		uid;
	double	grade;
};

struct LD_CHANGE_DEPARTMENT_PACKET :public SPACKET {
	char	department;
	int		uid;
};

struct DL_LOGIN_OK_PACKET :public SPACKET {
	int		uid;
	char	id[MAX_NAME_SIZE];
	char	nickname[MAX_NAME_SIZE];
	char	department;
	double	grade;
	int		point;
	int		userKey;
	bool	connState;
};

struct DL_LOGIN_FAIL_PACKET :public SPACKET {
	int		userKey;
};

struct DL_UPDATE_NICKNAME_OK_PACKET :public SPACKET {
	int		userKey;
};

struct DL_SIGNUP_OK_PACKET :public SPACKET {
	int		userKey;
};

struct DL_SIGNUP_FAIL_PACKET :public SPACKET {
	int		userKey;
};

#pragma pack (pop)