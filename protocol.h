//test ip
#pragma once
namespace proto {
	constexpr int MAX_COUPONLEN = 20;
}
constexpr const char* SERVERIP = "127.0.0.1";
constexpr const char* LOOBYSERVER_ADDR = "127.0.0.1";

constexpr int GAMESERVERPORT = 3600;
constexpr int LOBBYSERVERPORT = 3501;
constexpr int BUF_SIZE = 200;


constexpr int MAX_USER = 300;
constexpr int MAX_OBJECT = 3000;
constexpr int MAX_NAME_SIZE = 20;	//�г���, �ߺ��˻� X
constexpr int MAX_ROOM_USER = 8;
constexpr int MAX_VERSION_SIZE = 10;

// Object Event Time
constexpr int DELAY_TIME_EXEC_BREAKDOOR = 1000;
constexpr int DELAY_TIME_EXEC_BREAKPLATFORM = 3000;

// Level min, max
constexpr int MIN_LEVEL = 1;
constexpr int MAX_LEVEL = 8;

//item
constexpr long long HEADEQUIP = 0b0000'0000'0000'0000'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111;//48~63
constexpr long long FACEEQUIP = 0b1111'1111'1111'1111'0000'0000'0000'0000'1111'1111'1111'1111'1111'1111'1111'1111;//32~47
constexpr long long HANDEQUIP = 0b1111'1111'1111'1111'1111'1111'1111'1111'0000'0000'0000'0000'1111'1111'1111'1111;//16~31
constexpr long long BACKEQUIP = 0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'0000'0000'0000'0000;//0~15

constexpr int STARTCODE_HEADEQUIP = 48;
constexpr int STARTCODE_FACEEQUIP = 32;
constexpr int STARTCODE_HANDEQUIP = 16;
constexpr int STARTCODE_BACKEQUIP = 0;

// rank
constexpr int RANK_COUNT = 10;

struct rankInfo
{
	char	department	= 0;
	int		point		= 0;
};

enum class eDepartment : char
{

	None,
	Game,
	Ai,
	Computer,
	MechanicalEngine,
	MechanicalDesign,
	Mechatronics,
	Electrionics,
	AdvMaterial,
	Chemical,
	Nano,
	EnergyElec,
	Bussiness,
	Design,
	Staff
};

enum PacketType {
	// Client To GameServer
	CS_LOGIN,
	CS_MOVE,
	CS_CHAT,
	CS_GOAL,
	CS_PING,
	CS_ACTION,
	CS_GAME_PLAYERLOAD_ACK,
	CS_GAME_BREAKDOOR,
	CS_GAME_BREAKPLATFORM,

	// Client To LobbyServer
	CL_LOGIN,
	CL_LOGOUT,
	CL_SIGNUP,
	CL_MATCH,
	CL_MATCH_OUT,
	CL_CONTROL,
	CL_EQUIP_ITEM,
	CL_UNEQUIP_ITEM,
	CL_BUY_ITEM,
	CL_USE_COUPON,
	CL_REFRESH_INVENTORY,
	CL_REFRESH_DEP_RANK,
	CL_REFRESH_USERSTATUS,
	// ManageMent To LobbyServer
	ML_ALERT,

	// GameServer To Client
	SC_LOGIN_OK,
	SC_LOGIN_FAIL,
	SC_DISCONN,
	SC_ADD_PLAYER,
	SC_PLAYER_REMOVE,
	SC_MOVE_PLAYER,
	SC_PLAYER_ARRIVE,
	SC_GAME_WAITTING,
	SC_GAME_START,
	SC_GAME_END,
	SC_GAME_COUNTDOWN_START,
	SC_PING,
	SC_ACTION_ANIM,
	SC_GAME_DOORSYNC,
	SC_GAME_PLAYERLOAD_OK,
	SC_GAME_BREAKDOOR,
	SC_GAME_BREAKPLATFORM,

	// Lobbyserver To Client
	LC_LOGIN_OK,
	LC_LOGIN_FAIL,
	LC_SIGNUP_OK,
	LC_SIGNUP_FAIL,
	LC_MATCH_RESPONSE,
	LC_GAME_RESULT,
	LC_CONTROL,
	LC_BUYITEM_OK,
	LC_BUYITEM_FAIL,
	LC_REFRESH_INVENTORY,
	LC_REFRESH_DEP_RANK,
	LC_REFRESH_USERSTATUS,
	LC_EQUIP_OK,
	LC_UNEQUIP_OK,
	LC_USE_COUPON_OK,
	LC_USE_COUPON_FAIL,
	LC_ALERT,

	CL_CHECK_VERSION,
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
	int		roomID;	//������ lobbyServer���� ��� �ϴ� ��. ���߿� '������' ������. �Ǵ� �������� ����ϸ� ���� �ȵ�.
	char	hashs[MAX_NAME_SIZE];	//��ȣȭ ��

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

struct CS_GAME_PLAYERLOAD_ACK_PACKET : public PACKET {
	int temp[30];
};

struct CS_GAME_BREAKDOOR_PACKET : public PACKET {
	int	objectID;
};

struct CS_GAME_BREAKPLATFORM_PACKET : public PACKET {
	int	objectID;
};

//-----------------------------------
struct CL_LOGIN_PACKET :public PACKET {
	char id[MAX_NAME_SIZE];
	char password[MAX_NAME_SIZE];
	char gameVersion[MAX_VERSION_SIZE];
};

struct CL_LOGOUT_PACKET :public PACKET {

};

struct CL_SIGNUP_PACKET :public PACKET {
	char id[MAX_NAME_SIZE];
	char password[MAX_NAME_SIZE];
	char department;
};

struct CL_MATCH_PACKET : public PACKET {

};

struct CL_MATCH_OUT_PACKET : public PACKET {

};

struct CL_CONTROL_PACKET : public PACKET {

};

struct CL_EQUIP_ITEM_PACKET : public PACKET {
	int itemCode;
};

struct CL_UNEQUIP_ITEM_PACKET : public PACKET {
	int itemCode;
};

struct CL_BUY_ITEM_PACKET : public PACKET {
	int itemCode;
};

struct CL_USE_COUPON_PACKET :public PACKET {
	char couponCode[proto::MAX_COUPONLEN];
};

struct CL_REFRESH_INVENTORY_PACKET : public PACKET {

};

struct CL_REFRESH_DEP_RANK_PACKET : public PACKET {

};

struct CL_REFRESH_USERSTATUS_PACKET : public PACKET {
	
};

struct CL_CHECK_VERSION_PACKET : public PACKET {

	char gameVersion[MAX_VERSION_SIZE];
};
//-----------------------------------
struct ML_ALERT_PACKET : public PACKET {
	wchar_t alert[70]; //���������� ������ ����!
};

//-----------------------------------
struct SC_LOGIN_OK_PACKET : public PACKET {
	int		id;
	char	department;
	long long equipmentFlag;
};

struct SC_DISCONN_PACKET : public PACKET {
	int uid;
};

struct SC_MOVE_PLAYER_PACKET : public PACKET {
	int		id;
	float x, y, z;
	float rx, ry, rz, rw;
	float speed;
	bool	inair;
	float sx, sy, sz;		//speed for interpolation
	int		move_time;		//ping
};

struct SC_ADD_PLAYER_PACKET : public PACKET {
	int id;
	float x, y, z;
	float rx, ry, rz, rw;
	char	name[MAX_NAME_SIZE];
	char	department;
	long long equipmentFlag;

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

struct SC_GAME_WAITTING_PACKET : public PACKET {

};

struct SC_GAME_START_PACKET : public PACKET {

};

struct SC_GAME_END_PACKET : public PACKET {
	int temp[30];
	//char	record;		//���� ����
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

struct SC_GAME_PLAYERLOAD_OK_PACKET : public PACKET {
	int temp[30];
};

struct SC_GAME_BREAKDOOR_PACKET : public PACKET {
	int	objectID;
};

struct SC_GAME_BREAKPLATFORM_PACKET : public PACKET {
	int	objectID;
};
//---------------------------

struct LC_LOGIN_OK_PACKET : public PACKET {
	int id;
	int UID;
	double grade;
	int point;
	int RoomID;
	long long equippedItems;
	long long inventoryFlag;
	rankInfo ranking[10];
};

struct 	LC_LOGIN_FAIL_PACKET :public PACKET {

};

struct LC_SIGNUP_OK_PACKET :public PACKET {

};

struct LC_SIGNUP_FAIL_PACKET :public PACKET {

};

struct LC_MATCH_RESPONSE_PACKET : public PACKET {
	int gameServerPortNum;
	unsigned char gameServerIP[16];	//IPv4���� ip�� 4����Ʈ�ε� ���ڷ� ǥ���ϴϱ� �ִ� xxx.xxx.xxx.xxx  15 + �ι��� = 16
	char	hashs[MAX_NAME_SIZE];	//��ȣȭ �� �� ���� ������� client verification
	int		mapLevel;	//��� ° �� ����Ұ�����.
};

struct LC_GAME_RESULT_PACKET : public PACKET {
	int		rank;
	double	grade;
	int		point;
};

struct LC_CONTROL_PACKET :public PACKET {

};

struct LC_BUYITEM_OK_PACKET :public PACKET {
	int itemCode;
	int	pointAfterPurchase;
	long long inventoryFlag;
	long long equipmentFlag;
};

struct LC_BUYITEM_FAIL_PACKET :public PACKET {

};

struct LC_REFRESH_INVENTORY_PACKET : public PACKET {
	long long inventoryFlag;
	long long equipmentFlag;
};

struct LC_REFRESH_DEP_RANK_PACKET : public PACKET {
	rankInfo ranking[10];
};

struct LC_REFRESH_USERSTATUS_PACKET : public PACKET {
	int point;
	double grade;
};

struct LC_EQUIP_OK_PACKET : public PACKET {
	int itemCode;
	long long equipmentFlag;
};

struct LC_UNEQUIP_OK_PACKET : public PACKET {
	int itemCode;
	long long equipmentFlag;
};

struct LC_USE_COUPON_OK_PACKET :public PACKET {
	int		  itemcode;
	long long inventoryFlag;
	long long equipmentFlag;
};

struct LC_USE_COUPON_FAIL_PACKET :public PACKET {

};

struct LC_ALERT_PACKET : public PACKET {
	wchar_t alert[70];
};

//--------------------------
struct EVENT {
	unsigned char size;
	char	type;
};

#pragma pack (pop)