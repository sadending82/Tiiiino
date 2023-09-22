#pragma once

#include "stdafx.h"

class DB {

public:
	void SetmConn(MYSQL conn) { mConn = conn; }

	MYSQL* GetmConnection() const { return mConnection; }
	MYSQL  GetmConn() const { return mConn; }
	MYSQL_RES* GetmSqlResult() const { return mSqlResult; }
	MYSQL_ROW GetmSqlRow() const { return mSqlRow; }

	bool ConnectDB();
	bool ExecuteQuery(MYSQL_STMT* stmt);

	vector<string> SelectUserData(const string& UID);
	bool InsertNewUser(const string& id, const string& passWord, const string& nickname);
	bool UpdateUserNickname(const string& uid, const string& nicknameToChange);

	void DisconnectDB();

private:
	MYSQL*		mConnection = NULL;
	MYSQL		mConn;
	MYSQL_RES*	mSqlResult;
	MYSQL_ROW	mSqlRow;
	MYSQL_STMT* mStmt		= NULL;

	// юс╫ц
	const char* host = "localhost";
	const char* user = "root";
	const char* PW = "123qwe";
	const char* DBName = "Tiiiino";
};