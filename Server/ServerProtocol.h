#pragma once

namespace svproto {
	constexpr int MAX_COUPONLEN = 20;
}

constexpr const char* DBSERVERIP = "58.125.39.92";
constexpr int DBSERVERPORT = 3700;

constexpr int MAXGAMESERVER = 2;
constexpr int MAX_ROOM = 100;


#define CHECK_VERSION
constexpr const char* GAMEVERSION = "1.2.0";

constexpr unsigned long long INCODE_SERVER_PACKET = 0xFFFF'5137'0808'6310;

enum DB_ErrorCode {
	NONE			 = 0,
	SIGNUP_Duplicate = 101
};

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
	LD_UPDATE_GRADE,
	LD_CHANGE_DEPARTMENT,
	LD_EQUIP_ITEM,
	LD_UNEQUIP_ITEM,
	LD_BUY_ITEM,
	LD_USE_COUPON,
	LD_REFRESH_INVENTORY,
	LD_REFRESH_DEP_RANK,
	LD_GET_POINT,

	// DBServer To LobbyServer
	DL_LOGIN_OK,
	DL_LOGIN_FAIL,
	DL_CHANGE_DEPARTMENT_OK,
	DL_SIGNUP_OK,
	DL_SIGNUP_FAIL,
	DL_REFRESH_INVENTORY,
	DL_BUYITEM_OK,
	DL_BUYITEM_FAIL,
	DL_USE_COUPON_OK,
	DL_USE_COUPON_FAIL,
	DL_REFRESH_DEP_RANK,
	DL_EQUIP_OK,
	DL_UNEQUIP_OK,
	DL_GET_POINT,
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
	int		roomID;
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
	char	gameVersion[MAX_VERSION_SIZE];
	int		userKey;
};

struct LD_LOGOUT_PACKET :public SPACKET {
	int		uid;
};

struct LD_SIGNUP_PACKET :public SPACKET {
	char	id[MAX_NAME_SIZE];
	char	password[MAX_NAME_SIZE];
	char    department;
	int		userKey;
};

struct LD_UPDATE_GRADE_PACKET :public SPACKET {
	int		uid;
	double	grade;
	int		point;
};

struct LD_CHANGE_DEPARTMENT_PACKET :public SPACKET {
	char	department;
	int		uid;
};

struct LD_EQUIP_ITEM_PACKET :public SPACKET {
	int			uid;
	long long	itemCode;
	int			userKey;
};

struct LD_UNEQUIP_ITEM_PACKET :public SPACKET {
	int			uid;
	int			itemCode;
	int			userKey;
};

struct LD_BUY_ITEM_PACKET :public SPACKET {
	int			uid;
	int			itemCode;
	int			price;
	int			userKey;
};

struct LD_USE_COUPON_PACKET :public SPACKET {
	int		uid;
	char	couponCode[svproto::MAX_COUPONLEN];
	int		userKey;
};

struct LD_REFRESH_INVENTORY_PACKET :public SPACKET {
	int		uid;
	int		userKey;
};

struct LD_REFRESH_DEP_RANK_PACKET :public SPACKET {
	int		userKey;
};

struct LD_GET_POINT_PACKET : public SPACKET {
	int uid;
	int userKey;
};

struct DL_LOGIN_OK_PACKET :public SPACKET {
	int		uid;
	char	id[MAX_NAME_SIZE];
	char	department;
	double	grade;
	int		point;
	int		userKey;
	bool	connState;
	long long	equipmentflag;
	long long	inventoryflag;
	rankInfo ranking[10];
};

struct DL_LOGIN_FAIL_PACKET :public SPACKET {
	int		errorCode;
	int		userKey;
};

struct DL_SIGNUP_OK_PACKET :public SPACKET {
	int		userKey;
};

struct DL_SIGNUP_FAIL_PACKET :public SPACKET {
	int		errorCode;
	int		userKey;
};

struct DL_REFRESH_INVENTORY_PACKET :public SPACKET {
	long long	inventoryFlag;
	int			userKey;
};

struct DL_BUYITEM_OK_PACKET :public SPACKET {
	int		itemCode;
	int		pointAfterPurchase;
	int		userKey;
};

struct DL_BUYITEM_FAIL_PACKET :public SPACKET {
	int		userKey;
};

struct DL_USE_COUPON_OK_PACKET :public SPACKET {
	int		  itemcode;
	long long inventoryFlag;
	int		  userKey;
};

struct DL_USE_COUPON_FAIL_PACKET :public SPACKET {
	int		userKey;
};

struct DL_REFRESH_DEP_RANK_PACKET :public SPACKET {
	rankInfo ranking[10];
	int		userKey;
};

struct DL_EQUIP_OK_PACKET : public SPACKET {
	int itemCode;
	long long equipmentFlag;
	int userKey;
};

struct DL_UNEQUIP_OK_PACKET : public SPACKET{
	int itemCode;
	long long equipmentFlag;
	int userKey;
};

struct DL_GET_POINT_PACKET : public SPACKET {
	int point;
	int userKey;
};

#pragma pack (pop)