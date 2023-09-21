#pragma once
#include "DB.h"

#define Test

bool DB::ConnectDB()
{

	MYSQL conn;
	if (mysql_init(&conn) == NULL) {
#ifdef Test
		std::cout << "init fail" << std::endl;
#endif 

		return false;
	}

	SetmConn(conn);
	mConnecton = mysql_real_connect(&conn, host, user, PW, "Tiiiino", 3306, (const char*)NULL, 0);

	if (mConnecton == NULL) {
#ifdef Test
		std::cout << "connect fail" << std::endl;
#endif
		return false;
	}
	else {
#ifdef Test
		std::cout << "connect success" << std::endl;
#endif
	}

	if (mysql_select_db(mConnecton, "Tiiiino")) {
#ifdef Test
		std::cout << "select db fail" << std::endl;
#endif
		return false;
	}

	return true;
}

bool DB::ExecuteQuery(MYSQL_STMT* stmt)
{
	int state = mysql_stmt_execute(stmt);
	if (state != 0) {
#ifdef Test
		std::cout << "Query Execute error: " << mysql_stmt_error(stmt) << std::endl;
#endif
		return false;
	}

	return true;
}

vector<string> DB::SelectUserData(const string UID)
{
	vector<string> data;

	string query = "SELECT * FROM tiiiino.userinfo WHERE UID = " + UID;

	MYSQL_STMT* stmt = mysql_stmt_init(mConnecton);

	if (mysql_stmt_prepare(stmt, query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "stmt prepare error: " << mysql_stmt_error(stmt) << std::endl;
#endif
		return data;
	}

	MYSQL_BIND resultBinds[6];
	memset(resultBinds, 0, sizeof(resultBinds));
	const int colNum = 6;
	char bindData[colNum][50];
	for (int i = 0; i< colNum; ++i)
	{
		resultBinds[i].buffer_type = MYSQL_TYPE_STRING;
		resultBinds[i].buffer_length = sizeof(bindData[i]);
		resultBinds[i].buffer = bindData[i];
	}

	if (mysql_stmt_bind_result(stmt, resultBinds) != 0) {
#ifdef Test
		std::cout << "stmt bind error: " << mysql_stmt_error(stmt) << std::endl;
#endif
		return data;
	}

	if (ExecuteQuery(stmt) != false) {
		return data;
	}

	mysql_stmt_fetch(stmt);

	for (string col : bindData) {
		data.push_back(col);
	}

	return data;
}
