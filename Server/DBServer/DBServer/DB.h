#pragma once

#include "stdafx.h"
#include "cSecurity.h"

class DB {

public:
<<<<<<< Updated upstream:Server/DBServer/DBServer/DB.h
	void SetmConn(MYSQL conn)				{ mConn = conn; }
	void SetmConnection(MYSQL* connection)	{ mConnection = connection; }
	void SetmStmt(MYSQL_STMT* stmt)			{ mStmt = stmt; }

	MYSQL* GetmConnection() const			{ return mConnection; }
	MYSQL* GetmConn()			 			{ return &mConn; }
	MYSQL_STMT* GetmStmt() const			{ return mStmt; }
=======
	void SetmConn(MYSQL conn) { mConn = conn; }
	void SetmStmt(MYSQL_STMT* stmt) { mStmt = stmt; }
	void SetmSecurity(Security* sec) { mSecurity = sec; }

	MYSQL* GetmConn() { return &mConn; }
	MYSQL_STMT* GetmStmt() { return mStmt; }
	Security* GetmSecurity() { return mSecurity; }
>>>>>>> Stashed changes:Server/DBServer/DBServer/cDB.h

	const char* GetHost() { return mHost; }
	const char* GetUser() { return mUser; }
	const char* GetPassWord() { return mPW; }
	const char* GetDBName() { return mDBName; }
	const int	GetPort() { return mPort; }

	bool ConnectDB();
<<<<<<< Updated upstream:Server/DBServer/DBServer/DB.h
	bool ExecuteQuery(MYSQL_STMT* stmt);

	vector<string> SelectUserData(const string& UID);
	bool InsertNewUser(const string& id, const string& passWord, const string& nickname);
	bool UpdateUserNickname(const string& uid, const string& nicknameToChange);
=======
	bool ExecuteQuery();

	tuple<string, string, double, int> SelectUserData(const int uid);
	tuple<int, string, double, int, bool> SelectUserDataForLogin(const string& id);
	vector<string> SelectHash(const string& id); // Index 0: hash / Index 1: salt

	bool InsertNewUser(const string& id);
	bool InsertNewAccount(const string& id, const string& password);

	bool UpdateUserConnectionState(const int uid, const int state);
	bool UpdateUserNickname(const int uid, const string& nicknameToChange);
	bool UpdateUserCredit(const int uid, double credit);
	bool UpdateUserPoint(const int uid, unsigned int point);
>>>>>>> Stashed changes:Server/DBServer/DBServer/cDB.h

	bool DeleteAccount(const string& id);

	bool SignUpNewPlayer(const string& id, const string& password);
	bool CheckVerifyUser(const string& id, const string& password);

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

	Security* mSecurity = NULL;
};