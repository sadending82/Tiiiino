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
	mConnection = mysql_real_connect(&conn, host, user, PW, "Tiiiino", 3306, (const char*)NULL, 0);

	if (mConnection == NULL) {
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

	if (mysql_select_db(mConnection, "Tiiiino")) {
#ifdef Test
		std::cout << "select db fail" << std::endl;
#endif
		return false;
	}
	mStmt = mysql_stmt_init(mConnection);

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

vector<string> DB::SelectUserData(const string& UID)
{
	vector<string> data;

	string query = "SELECT * FROM tiiiino.userinfo WHERE UID = " + UID;

	if (mysql_stmt_prepare(mStmt, query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt prepare error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return data;
	}

	const int colNum = 6;
	MYSQL_BIND resultBinds[colNum];
	memset(resultBinds, 0, sizeof(resultBinds));
	char bindData[colNum][50];
	for (int i = 0; i< colNum; ++i)
	{
		resultBinds[i].buffer_type = MYSQL_TYPE_STRING;
		resultBinds[i].buffer_length = sizeof(bindData[i]);
		resultBinds[i].buffer = bindData[i];
	}

	if (mysql_stmt_bind_result(mStmt, resultBinds) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return data;
	}

	if (ExecuteQuery(mStmt) != false) {
		return data;
	}

	mysql_stmt_fetch(mStmt);

	for (string col : bindData) {
		data.push_back(col);
	}

	return data;
}

bool DB::InsertNewUser(const string& id, const string& passWord, const string& nickname)
{
	string query = "INSERT INTO userinfo (ID, PassWord, nick) VALUES (?, ?, ?)";

	if (mysql_stmt_prepare(mStmt, query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "InsertNewUser stmt prepare error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	const int colNum = 3;

	MYSQL_BIND binds[colNum];
	memset(binds, 0, sizeof(binds));

	binds[0].buffer_type = MYSQL_TYPE_STRING;
	binds[0].buffer = (void*)id.c_str();
	binds[0].buffer_length = id.length();

	binds[1].buffer_type = MYSQL_TYPE_STRING;
	binds[1].buffer = (void*)passWord.c_str();
	binds[1].buffer_length = passWord.length();

	binds[2].buffer_type = MYSQL_TYPE_STRING;
	binds[2].buffer = (void*)nickname.c_str();
	binds[2].buffer_length = nickname.length();

	if (mysql_stmt_bind_param(mStmt, binds) != 0) {

#ifdef Test
			std::cout << "InsertNewUser stmt bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery(mStmt) != false) {
		return false;
	}

	return true;
}

void DB::DisconnectDB()
{
	mysql_close(mConnection);
	mysql_stmt_close(mStmt);
}
