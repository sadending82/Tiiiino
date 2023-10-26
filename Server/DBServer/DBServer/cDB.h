#pragma once

#include "stdafx.h"
#include "cSecurity.h"

#ifdef RUN_DB

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

	tuple<ID, GRADE, POINT> SelectUserData(const int uid);
	tuple<UNIQUEID, GRADE, POINT, STATE, DEPARTMENT> SelectUserDataForLogin(const string& id);
	vector<string> SelectHash(const string& id); // Index 0: hash / Index 1: salt
	tuple<GRADE, DEPARTMENT> SelectUserGradeAndDepartment(const int uid);
	long long SelectInventory(const int uid);

	bool InsertNewUser(const string& id, const char department);
	bool InsertNewAccount(const string& id, const string& password);
	bool InsertNewInventory(const int uid);

	bool UpdateUserConnectionState(const int uid, const int state);
	bool UpdateUserGrade(const int uid, double grade);
	bool UpdateUserPoint(const int uid, unsigned int point);
	bool UpdateRanking(const char department, const int incrementPoint);
	bool UpdateUserDepartment(const int uid, const char department);
	bool UpdateInventory(const int uid, const int itemCode);

	bool DeleteAccount(const string& id);
	bool DeleteItemInInventory(const int uid, const int itemCode);

	bool SignUpNewPlayer(const string& id, const string& password, const char department);
	bool CheckVerifyUser(const string& id, const string& password);

	void DisconnectDB();

	void CreateDummyAccount(int count);

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

#endif