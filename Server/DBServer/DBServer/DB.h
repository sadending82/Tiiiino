#pragma once

#include "stdafx.h"

class DB {

public:
	void SetmConn(MYSQL conn)				{ mConn = conn; }
	void SetmConnection(MYSQL* connection)	{ mConnection = connection; }
	void SetmStmt(MYSQL_STMT* stmt)			{ mStmt = stmt; }

	MYSQL* GetmConnection() const			{ return mConnection; }
	MYSQL* GetmConn()			 			{ return &mConn; }
	MYSQL_STMT* GetmStmt() const			{ return mStmt; }

	const char* GetHost()					{ return mHost; }
	const char* GetUser()					{ return mUser; }
	const char* GetPassWord()				{ return mPW; }
	const char* GetDBName()					{ return mDBName; }
	const int	GetPort()					{ return mPort; }

	bool ConnectDB();
	bool ExecuteQuery(MYSQL_STMT* stmt);

	vector<string> SelectUserData(const string& UID);
	bool InsertNewUser(const string& id, const string& passWord, const string& nickname);
	bool UpdateUserNickname(const string& uid, const string& nicknameToChange);

	void DisconnectDB();

private:
	MYSQL*		mConnection = NULL;
	MYSQL		mConn;
	MYSQL_STMT* mStmt		= NULL;

	// юс╫ц
	const char* mHost = "localhost";
	const char* mUser = "root";
	const char* mPW = "123qwe";
	const char* mDBName = "Tiiiino";
	const int   mPort = 3306;
};