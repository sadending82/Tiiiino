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
	~MainServer();

	std::array<class Object*, MAX_OBJECT>& getObjects() { return mObjects; }

	void ProcessPacket(const int clientID, unsigned char* p);
	int GenerateID();

	HANDLE GetIOCPHandle() const { return mhiocp; }

	void init();
	void run();

	void send_login_ok_packet(int player_id, const char* playername);
	void send_move_packet(int player_id, int mover_id, const bool& inair, float value, const float& sx, const float& sy, const float& sz);
private:
	std::array<class Object*, MAX_OBJECT> mObjects;
	std::map<int, Room*> mRooms;
	//std::array< std::vector<class Object*>, MAX_ROOM> mRooms;
	std::vector<std::thread> mWorkerThreads;
	class WorkerThread* mWorkerThreadRef;
	HANDLE mhiocp;
};

