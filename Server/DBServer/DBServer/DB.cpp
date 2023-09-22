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
	SetmConnection(mysql_real_connect(&conn, GetHost(), GetUser(), GetPassWord(), GetDBName(), GetPort(), (const char*)NULL, 0));

	if (GetmConnection() == NULL) {
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

	if (mysql_select_db(GetmConnection(), GetDBName())) {
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

vector<string> DB::SelectUserData(const string& UID)
{
	vector<string> data;

	string query = "SELECT * FROM tiiiino.userinfo WHERE UID = " + UID;

	SetmStmt(mysql_stmt_init(GetmConnection()));

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
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

	if (mysql_stmt_bind_result(GetmStmt(), resultBinds) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return data;
	}

	if (ExecuteQuery(GetmStmt()) != false) {
		return data;
	}

	mysql_stmt_fetch(GetmStmt());

	for (string col : bindData) {
		data.push_back(col);
	}

	return data;
}

bool DB::InsertNewUser(const string& id, const string& passWord, const string& nickname)
{
	string query = "INSERT INTO userinfo (ID, PassWord, nick) VALUES (?, ?, ?)";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
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

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {

#ifdef Test
			std::cout << "InsertNewUser stmt bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery(GetmStmt()) != false) {
		return false;
	}

	return true;
}

bool DB::UpdateUserNickname(const string& uid, const string& nicknameToChange)
{
	string query = "UPDATE userinfo SET nick = ? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "InsertNewUser stmt prepare error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	const int colNum = 2;

	MYSQL_BIND binds[colNum];
	memset(binds, 0, sizeof(binds));

	binds[0].buffer_type = MYSQL_TYPE_STRING;
	binds[0].buffer = (void*)nicknameToChange.c_str();
	binds[0].buffer_length = nicknameToChange.length();

	int iuid = stoi(uid);
	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = &iuid;

	if (mysql_stmt_bind_param(mStmt, binds) != 0) {

#ifdef Test
		std::cout << "UpdateUserNickname stmt bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery(GetmStmt()) != false) {
		return false;
	}

	return true;
}

void DB::DisconnectDB()
{
	mysql_stmt_close(GetmStmt());
	mysql_close(GetmConn());
}
