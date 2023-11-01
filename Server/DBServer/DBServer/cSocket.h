#pragma once
#include "stdafx.h"
#include "cSession.h"
#include "cDB.h"
#include "../../ServerProtocol.h"

class Socket {
public:

	int SetKey();
	void Disconnect(int key);
	void WorkerFunc();
	void ServerReady();
	void processPacket(int key, unsigned char* buf);

#ifdef RUN_DB
	void Setm_pDB(DB* pDB) { m_pDB = pDB; }
	DB* Getm_pDB() { return m_pDB; }
	void SetIsConnectDB(bool b) { bIsConnectDB = b; }
	bool GetIsConnectDB() { return bIsConnectDB; }
#endif
	bool CheckVersion(const char* version);
	bool CheckLogin(int key, const char* id, const char* password, int userid, const char* version);
	bool CheckValidString(const char* str);

	long long EquipItem(int itemCode, long long equipmentFlag);
	long long UnequipItem(int itemCode, long long equipmentFlag);

	void SendLoginOK(int key, int uid, const char* id, double grade
		, int point, int state, char department
		, long long equippedItemFlag, long long inventoryFlag,
		vector<rankInfo>& ranking, int userid);
	void SendLoginFail(int key, int userKey);
	void SendSignUpOK(int key, int userKey);
	void SendSignUpFail(int key, int userKey);
	void SendInventory(int key, long long inventoryFlag, int userKey);
	void SendBuyItemOK(int key, int itemCode, int userKey, int pointAfterPurchase);
	void SendBuyItemFail(int key, int userKey);
	void SendUseCouponOK(int key, int itemCode, int userKey, long long inventoryFlag);
	void SendUseCouponFail(int key, int userKey);
	void SendRanking(int key, vector<rankInfo>& ranking, int userKey);
	void SendEquipOK(int key, long long equipmentFlag, int userkey);
	void SendUnequipOK(int key, long long equipmentFlag, int userkey);

	void ProcessPacket_Login(int key, unsigned char* buf);
	void ProcessPacket_Logout(unsigned char* buf);
	void ProcessPacket_SignUp(int key, unsigned char* buf);
	void ProcessPacket_UpdateGrade(int key, unsigned char* buf);
	void ProcessPacket_RefreshInventory(int key, unsigned char* buf);
	void ProcessPacket_ChangeDepartment(int key, unsigned char* buf);
	void ProcessPacket_EquipItem(int key, unsigned char* buf);
	void ProcessPacket_UnequipItem(int key, unsigned char* buf);
	void ProcessPacket_BuyItem(int key, unsigned char* buf);
	void ProcessPacket_UseCoupon(int key, unsigned char* buf);
	void ProcessPacket_RefreshDepRank(int key, unsigned char* buf);

	int SetUIDForTest();

private:
	SOCKADDR_IN mServerAddr;
	HANDLE mHcp;
	SOCKET mListenSocket;

#ifdef RUN_DB
	DB* m_pDB = NULL;
	bool bIsConnectDB = false;
#endif

	array<Session, MAXLOBBY + 1> mSessions;

	int testUID = 1;
};