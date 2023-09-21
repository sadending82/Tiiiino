#pragma once

#include "stdafx.h"

class DB {

public:
	void SetmConn(MYSQL conn) { mConn = conn; }

	MYSQL* GetmConnection() const { return mConnecton; }
	MYSQL  GetmConn() const { return mConn; }
	MYSQL_RES* GetmSqlResult() const { return mSqlResult; }
	MYSQL_ROW GetmSqlRow() const { return mSqlRow; }

	bool ConnectDB();
	bool ExecuteQuery(MYSQL_STMT* stmt);
	vector<string> SelectUserData(const string UID);


private:
	MYSQL* mConnecton = NULL;
	MYSQL mConn;
	MYSQL_RES* mSqlResult;
	MYSQL_ROW mSqlRow;

	const char* host = "localhost";
	const char* user = "root";
	const char* PW = "123qwe";
	const char* DBName = "Tiiiino";
};