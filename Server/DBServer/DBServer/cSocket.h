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

	bool CheckLogin(int key, const char* id, const char* password, int userid);
	bool CheckValidString(const char* str);

	void SendLoginOK(int key, int uid, string& nickname, const char* id, double grade
		, int point, int state, char department, int userid);
	void SendLoginFail(int key, int userKey);
	void SendSignUpOK(int key, int userKey);
	void SendSignUpFail(int key, int userKey);
	void SendUpdateNicknameOK(int key, int userKey);

	void ProcessPacket_Login(int key, unsigned char* buf);
	void ProcessPacket_Logout(unsigned char* buf);
	void ProcessPacket_SignUp(int key, unsigned char* buf);
	void ProcessPacket_UpdateNickname(int key, unsigned char* buf); 
	void ProcessPacket_UpdateGrade(int key, unsigned char* buf);

	void ProcessPacket_ChangeDepartment(int key, unsigned char* buf);

	int SetAdminUID();
	void Admin_Login(int key, unsigned char* buf);

private:
	SOCKADDR_IN mServerAddr;
	HANDLE mHcp;
	SOCKET mListenSocket;

#ifdef RUN_DB
	DB* m_pDB = NULL;
	bool bIsConnectDB = false;
#endif

	array<Session, MAXLOBBY + 1> mSessions;

	bool bAdminLogin[11];
};