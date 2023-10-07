#pragma once
#include <array>
#include <vector>
#include <map>

#include "../Server.h"


extern class MainServer* gMainServer;

class Room;
class MainServer : public Server
{
public:
	MainServer();
	virtual ~MainServer();

	std::array<class Object*, MAX_OBJECT>& GetObjects() { return mObjects; }
	std::map<int, Room*>& GetRooms() { return mRooms; }
	class Server* GetLobbyServer() { return mLobbyServer; }

	void ProcessPacket(const int clientID, unsigned char* p);
	void ProcessPacketLobby(const int serverID, unsigned char* p);
	int GenerateID();

	HANDLE GetIOCPHandle() const { return mhiocp; }

	void init();
	void run();

	//Server to Server
	void send_room_ready_packet(const int roomID);
	void send_room_reset_packet(const int roomID);

	//Server to client
	void SendPacketWithID(const int receiverID, void* buf, const int bufSize);
	void send_login_ok_packet(const int player_id, const char* playername);
	void send_login_ok_packet(const int player_id, void* buf);
	SC_LOGIN_OK_PACKET make_login_ok_packet(const int playerID, const char* playername);
	void send_player_add_packet(const int playerID, void* buf, const int bufSize);
	SC_ADD_PLAYER_PACKET make_player_add_packet(const int playerSocketID);
	SC_PLAYER_REMOVE_PACKET make_player_remove_packet(const int playerRoomSyncID);
	void send_move_packet(const int player_id, const int mover_id, const bool inair, const float value, const float sx, const float sy, const float sz);
	SC_MOVE_PLAYER_PACKET make_move_packet(const int moverSocketID, const bool inair, const float value, const float sx, const float sy, const float sz);
	 
	void send_player_arrive_packet(const int player_id,const int arrive_id);
	SC_PLAYER_ARRIVE_PACKET make_player_arrive_packet(const int arriveID);
	void send_game_countdown_start_packet(const int player_id);
	SC_GAME_COUNTDOWN_START_PACKET make_game_countdown_start_packet();
	SC_PING_PACKET make_ping_packet();
	SC_ACTION_ANIM_PACKET make_action_packet(const int playerID, const char action);
	SC_GAME_WAITTING_PACKET make_game_watting_packet();
	SC_GAME_START_PACKET make_game_start_packet();
	SC_GAME_END_PACKET make_game_end_packet();
	SC_GAME_DOORSYNC_PACKET make_game_doorsync_packet(const int objectID, const long long syncTime);
	SC_GAME_PLAYERLOAD_OK_PACKET make_game_playerload_ok_packet();
	SC_GAME_BREAKDOOR_PACKET make_game_breakdoor_packet(const int objectID);
	SC_GAME_BREAKPLATFORM_PACKET make_game_breakplatform_packet(const int objectID);

	//-> �����غ��� �׳� buffer�� ��Ƽ� void*�� ������ send���� ��ȯ�ϸ� ���ݾ� ? �ٷ� �������
	void SendAllBroadCast(void* buf, const int bufSize);
	void SendRoomBroadCast(const int roomID, void* buf, const int bufSize);
	void SendRoomSomeoneExcept(const int roomID, const int exceptSocketID, void* buf, const int bufSize);
	void SendMySelf(const int receiverSocketID, void* buf, const int bufSize);

	//���� ������ ������ �� ���� �� �Լ��� ���, ��� �� �Լ��� �� ���� ��Ŷ����� �Լ�(make_~)�� id �ϳ��� ����� �� �ְԲ� §��.
	//other info to owner, use this func, when you use this function, you must write the packet creation function (make_~) parameter just one [const int id].
	template<class T>
	void SendRoomOthersToMe(const int roomID,const int receiverSocketID, const int exceptSocketID, T(MainServer::* fp)(const int));
private:
	void connectLobbyServer();
	bool setPlayerInRoom(class Player* player,const char verification[MAX_NAME_SIZE]);
	bool initRoom(const std::string& mapName);
private:
	std::array<class Object*, MAX_OBJECT> mObjects;
	class Server* mLobbyServer;

	std::map<int, Room*> mRooms;
	//std::array< std::vector<class Object*>, MAX_ROOM> mRooms;
	std::vector<std::thread> mWorkerThreads;
	std::thread mTimerThread;
	class WorkerThread* mWorkerThreadRef;
	class TimerThread* mTimerThreadRef;
	HANDLE mhiocp;
};
