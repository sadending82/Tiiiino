#pragma once
#include "Session.h"
#include "Timer.h"
#include "room.h"
#include "../ServerProtocol.h"
#include "GameDataManager.h"

constexpr int MAX_THREAD = 6;
constexpr int INVALID_KEY = -1;

class Server {
public:
	void Disconnect(int cID);
	void DisconnectForVersionCheck(int cID);
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
	void BuyItem(int cID, int itemCode);

	void AddItemToInventory(int cID, int itemCode);

	void CheckDuplicateLogin(int uid);
	bool CheckAbleEquipItem(int cID, int itemCode);
	bool CheckAbleUnequipItem(int cID, int itemCode);

	void SendPlayerResult(int uID, int roomID, bool retire, int rank);

	// lobby to db
	void SendLogin(int cID, char* id, char* pass, char* version);
	void SendSignUp(int cID, char* id, char* pass, char depart);
	void SendGameResult(int roomID, int key);
	void SendEquipItem(int cID, int itemCode);
	void SendUnequipItem(int cID, int itemCode);
	void SendBuyItem(int cID, int itemCode);
	void SendRefreshRankingRequest(int cID);
	void SendUseCoupon(int cID, char* couponCode);

	void SendLoginOK(int cID, const rankInfo* rank);

	void SendDiconnectPacketToGameServer(int key, int uid, int roomID);
	void SendMatchResult(int key, int rank, int point);
	void SendSignUpOK(int key);
	void SendSignUpFail(int key);
	void SendMatchResponse(int roomID);
	void SendBuyOK(int key, int itemCode);
	void SendBuyFail(int key);
	void SendInventory(int key);
	void SendRankingToClient(int key, rankInfo* ranking);
	void SendEquipItemOK(int key, int itemCode, long long equipmentFlag);
	void SendUnequipItemOK(int key, int itemCode, long long equipmentFlag);
	void SendUseCouponOK(int key, int itemcode, long long inventoryFlag);
	void SendUseCouponFail(int key);

	void LoadGameData();
public:
	array <Session, MAX_USER> mClients;
	array <Session, MAXGAMESERVER> mServers;
	array <Room, MAX_ROOM> mRooms;
	//list <int> mMatchListHighTier;
	list <int> mMatchListLowTier;
	list <int> mReadytoGame;
	mutex mHighListLock;
	mutex mLowListlock;
private:
	SOCKET mListenSocket;
	HANDLE mHCP;
	vector <thread> mWorkerThreads;
	Timer* pTimer;

	GameDataManager* pGameDataManager;
};