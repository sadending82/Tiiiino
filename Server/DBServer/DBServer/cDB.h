#pragma once

#include "stdafx.h"

class DB {

public:
	const char* GetHost()					{ return mHost; }
	const char* GetUser()					{ return mUser; }
	const char* GetPassWord()				{ return mPW; }
	const char* GetDBName()					{ return mDBName; }
	const int	GetPort()					{ return mPort; }

	bool ConnectDB();
	/*bool ExecuteQuery();

	tuple<string, string, double, int> SelectUserData(const int uid);
	tuple<int, string, double, int, bool> SelectUserDataForLogin(const string& id, const string& password);

	bool InsertNewUser(const string& id, const string& passWord, const string& nickname);

	bool UpdateUserConnectionState(const int uid, const bool state);
	bool UpdateUserNickname(const int uid, const string& nicknameToChange);
	bool UpdateUserCredit(const int uid, double credit);
	bool UpdateUserPoint(const int uid, unsigned int point);

	void DisconnectDB();*/

private:
	sql::mysql::MySQL_Driver* mDriver;
	sql::Connection* mConn;

	// юс╫ц
	const char* mHost = "tcp://127.0.0.1:3306";
	const char* mUser = "root";
	const char* mPW = "123qwe";
	const char* mDBName = "Tiiiino";
	const int   mPort = 3306;
};