#pragma once

#include "stdafx.h"

class DB {

public:
	void SetmConn(MYSQL conn)				{ mConn = conn; }
	void SetmStmt(MYSQL_STMT* stmt)			{ mStmt = stmt; }

	MYSQL* GetmConn()			 			{ return &mConn; }
	MYSQL_STMT* GetmStmt()					{ return mStmt; }

	const char* GetHost()					{ return mHost; }
	const char* GetUser()					{ return mUser; }
	const char* GetPassWord()				{ return mPW; }
	const char* GetDBName()					{ return mDBName; }
	const int	GetPort()					{ return mPort; }

	bool ConnectDB();
	bool ExecuteQuery();

	vector<string> SelectUserData(const int uid);
	tuple<int, string, double, int> SelectUserData(const string& id, const string& password);
	bool InsertNewUser(const string& id, const string& passWord, const string& nickname);
	bool UpdateUserNickname(const int uid, const string& nicknameToChange);
	bool UpdateUserCredit(const int uid, double credit);
	bool UpdateUserPoint(const int uid, unsigned int point);

	void DisconnectDB();

private:
	MYSQL		mConn;
	MYSQL_STMT* mStmt;

	// юс╫ц
	const char* mHost = "localhost";
	const char* mUser = "root";
	const char* mPW = "123qwe";
	const char* mDBName = "Tiiiino";
	const int   mPort = 3306;
};