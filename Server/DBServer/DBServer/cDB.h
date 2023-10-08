#pragma once

#include "stdafx.h"
#include "cSecurity.h"

class DB {

public:
	void SetmConn(MYSQL conn) { mConn = conn; }
	void SetmStmt(MYSQL_STMT* stmt) { mStmt = stmt; }
	void SetmSecurity(Security* sec) { mSecurity = sec; }

	MYSQL* GetmConn() { return &mConn; }
	MYSQL_STMT* GetmStmt() { return mStmt; }
	Security* GetmSecurity() { return mSecurity; }

	const char* GetHost() { return mHost; }
	const char* GetUser() { return mUser; }
	const char* GetPassWord() { return mPW; }
	const char* GetDBName() { return mDBName; }
	const int	GetPort() { return mPort; }

	bool ConnectDB();
	bool ExecuteQuery();

	tuple<string, string, double, int> SelectUserData(const int uid);
	tuple<int, string, double, int, bool, char> SelectUserDataForLogin(const string& id);
	vector<string> SelectHash(const string& id); // Index 0: hash / Index 1: salt

	bool InsertNewUser(const string& id, const char department);
	bool InsertNewAccount(const string& id, const string& password);

	bool UpdateUserConnectionState(const int uid, const int state);
	bool UpdateUserNickname(const int uid, const string& nicknameToChange);
	bool UpdateUserGrade(const int uid, double grade);
	bool UpdateUserPoint(const int uid, unsigned int point);
	bool UpdateUserDepartment(const int uid, const char department);

	bool DeleteAccount(const string& id);

	bool SignUpNewPlayer(const string& id, const string& password, const char department);
	bool CheckVerifyUser(const string& id, const string& password);

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

	Security* mSecurity = NULL;
};