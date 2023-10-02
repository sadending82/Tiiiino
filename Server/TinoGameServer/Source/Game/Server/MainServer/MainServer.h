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

	void SendPacketWithID(const int receiverID, void* buf, const int bufSize);
	void send_login_ok_packet(const int player_id, const char* playername);
	void send_login_ok_packet(const int player_id, void* buf);
	SC_LOGIN_OK_PACKET make_login_ok_packet(const int playerID, const char* playername);
	void send_player_add_packet(const int playerID, void* buf, const int bufSize);
	SC_ADD_PLAYER_PACKET make_player_add_packet(const int playerID);
	void send_room_ready_packet(const int roomID);
	void send_move_packet(const int player_id, const int mover_id, const bool inair, const float value, const float sx, const float sy, const float sz);
	SC_MOVE_PLAYER_PACKET make_move_packet(const int moverID, const bool inair, const float value, const float sx, const float sy, const float sz);
	 
	void send_player_arrive_packet(const int player_id,const int arrive_id);
	SC_PLAYER_ARRIVE_PACKET make_player_arrive_packet(const int arriveID);
	void send_game_countdown_start_packet(const int player_id);
	SC_GAME_COUNTDOWN_START_PACKET make_game_countdown_start_packet();
	SC_PING_PACKET make_ping_packet();
	SC_ACTION_ANIM_PACKET make_action_packet(const int playerID, const char action);
	SC_GAME_END_PACKET make_game_end_packet(const char record);

	//-> 생각해보니 그냥 buffer에 담아서 void*로 보내고 send에서 변환하면 되잖아 ? 바로 진행시켜
	void SendAllBroadCast(void* buf, const int bufSize);
	void SendRoomBroadCast(const int roomID, void* buf, const int bufSize);
	void SendRoomSomeoneExcept(const int roomID, const int exceptID, void* buf, const int bufSize);
	void SendMySelf(const int receiverID, void* buf, const int bufSize);

	//남의 정보를 나한테 줄 때는 이 함수를 사용, 대신 이 함수를 쓸 때는 패킷만드는 함수(make_~)를 id 하나로 사용할 수 있게끔 짠다.
	//other info to owner, use this func, when you use this function, you must write the packet creation function (make_~) parameter just one [const int id].
	template<class T>
	void SendRoomOthersToMe(const int roomID,const int receiveID, const int exceptID, T(MainServer::* fp)(const int));
private:
	void connectLobbyServer();
	bool setPlayerInRoom(class Player* player);
private:
	std::array<class Object*, MAX_OBJECT> mObjects;
	class Server* mLobbyServer;

	std::map<int, Room*> mRooms;
	//std::array< std::vector<class Object*>, MAX_ROOM> mRooms;
	std::vector<std::thread> mWorkerThreads;
	class WorkerThread* mWorkerThreadRef;
	HANDLE mhiocp;
};
