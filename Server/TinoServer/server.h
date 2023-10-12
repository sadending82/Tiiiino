#pragma once
#include "Session.h"
#include "Timer.h"
#include "room.h"
#include "../ServerProtocol.h"

constexpr int MAX_THREAD = 6;
constexpr int INVALID_KEY = -1;

class Server {
public:
	void Disconnect(int cID);
	int GetNewClientID();
	int GetNewServerID();
	int GetNewRoomID();
	void ProcessPacket(int cID, unsigned char* cpacket);
	void ProcessPacketServer(int sID, unsigned char* spacket);
	void DoWorker();
	void Init();
	void ProcessEvent(unsigned char* cmessage);
	void PlayerMatchIn(int cID);
	void PlayerMatchOut(int cID);
	void RoomReset(int roomID);

	void CheckDuplicateLogin(int uid);


	void SendPlayerResult(int uID, int roomID, bool retire, int rank);

	// lobby to db
	void SendLogin(int key, char* id, char* pass);
	void SendSignUp(int key, char* id, char* pass, char depart);
	void SendGameResult(int roomID, int key);


	void SendLoginOK(int cID);

	void SendDiconnectPacketToGameServer(int key, int uid, int roomID);
	void SendMatchResult(int key, int rank, int point);
	void SendSignUpOK(int key);
	void SendSignUpFail(int key);
	void SendMatchResponse(int roomID);
public:
	array <Session, MAX_USER> mClients;
	array <Session, MAXGAMESERVER> mServers;
	array <Room, MAX_ROOM> mRooms;
	list <int> mMatchListHighTier;
	list <int> mMatchListLowTier;
	list <int> mReadytoGame;
	mutex mHighListLock;
	mutex mLowListlock;
private:
	SOCKET mListenSocket;
	HANDLE mHCP;
	vector <thread> mWorkerThreads;
	Timer* pTimer;
};