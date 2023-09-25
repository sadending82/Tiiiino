#pragma once
#include "cDB.h"

#define Test

bool DB::ConnectDB()
{
	if (mysql_init(&mConn) == NULL) {
#ifdef Test
		std::cout << "Mysql Handle Init Fail" << std::endl;
#endif 

		return false;
	}

	if (mysql_real_connect(&mConn, GetHost(), GetUser(), GetPassWord(), GetDBName(), GetPort(), (const char*)NULL, 0) != 0) {
#ifdef Test
		std::cout << "DataBase Connect Success" << std::endl;
		mysql_query(&mConn, "set names euckr");
#endif
	}
	else{
#ifdef Test
		std::cout << "DataBase Connect Fail" << std::endl;
#endif
		return false;
	}

	if (mysql_select_db(&mConn, GetDBName())) {
#ifdef Test
		std::cout << "Select DataBase Fail" << std::endl;
#endif
		return false;
	}

	mStmt = mysql_stmt_init(&mConn);


	return true;
}

bool DB::ExecuteQuery()
{
	int state = mysql_stmt_execute(mStmt);
	if (state != 0) {
#ifdef Test
		std::cout << "Query Execute error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	return true;
}

vector<string> DB::SelectUserData(const int uid)
{
	vector<string> data;

	string query = "SELECT id, nick, credit, point FROM userinfo WHERE uid = ?";

	if (mysql_stmt_prepare(mStmt, query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt prepare error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return vector<string>();
	}

	MYSQL_BIND paramBind;
	paramBind.buffer_type = MYSQL_TYPE_LONG;
	paramBind.buffer = (void*)&uid;

	if (mysql_stmt_bind_param(mStmt, &paramBind) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt param bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return vector<string>();
	}

	const int resColNum = 4;
	MYSQL_BIND resultBinds[resColNum];
	memset(resultBinds, 0, sizeof(resultBinds));
	char bindData[resColNum][50];
	for (int i = 0; i < resColNum; ++i)
	{
		resultBinds[i].buffer_type = MYSQL_TYPE_STRING;
		resultBinds[i].buffer_length = sizeof(bindData[i]);
		resultBinds[i].buffer = bindData[i];
	}

	if (mysql_stmt_bind_result(mStmt, resultBinds) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt result bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return vector<string>();
	}

	if (ExecuteQuery() == false) {
		return vector<string>();
	}

	if (mysql_stmt_fetch(mStmt) != 0) {
		return vector<string>();
	}

	for (string col : bindData) {
		data.push_back(col);
		cout << col << endl;
	}

	return data;
}

tuple<int, string, double, int, bool> DB::SelectUserDataForLogin(const string& id, const string& password)
{
	string query = "SELECT UID, nick, credit, point FROM tiiiino.userinfo WHERE id = ? AND password = ?";

	if (mysql_stmt_prepare(mStmt, query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt prepare error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return make_tuple(INVALIDKEY, "", 0.0, 0, false);
	}

	const int paramColNum = 2;
	MYSQL_BIND paramBinds[paramColNum];
	memset(paramBinds, 0, sizeof(paramBinds));

	paramBinds[0].buffer_type = MYSQL_TYPE_STRING;
	paramBinds[0].buffer = (void*)id.c_str();
	paramBinds[0].buffer_length = id.length();

	paramBinds[1].buffer_type = MYSQL_TYPE_STRING;
	paramBinds[1].buffer = (void*)password.c_str();
	paramBinds[1].buffer_length = password.length();

	if (mysql_stmt_bind_param(mStmt, paramBinds) != 0) {
#ifdef Test
			std::cout << "SelectUserData stmt param bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return make_tuple(INVALIDKEY, "", 0.0, 0, false);
	}

	const int resColNum = 5;
	MYSQL_BIND resultBinds[resColNum];
	memset(resultBinds, 0, sizeof(resultBinds));
	int bindUID, bindPoint;
	char bindNickname[MAX_NAME_SIZE];
	double bindCredit;
	bool bindState;
	{
		resultBinds[0].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[0].buffer = &bindUID;

		resultBinds[1].buffer_type = MYSQL_TYPE_STRING;
		resultBinds[1].buffer_length = sizeof(bindNickname);
		resultBinds[1].buffer = bindNickname;

		resultBinds[2].buffer_type = MYSQL_TYPE_DOUBLE;
		resultBinds[2].buffer = &bindCredit;

		resultBinds[3].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[3].buffer = &bindPoint;

		resultBinds[4].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[4].buffer = &bindState;
	}

	if (mysql_stmt_bind_result(mStmt, resultBinds) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt result bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return make_tuple(INVALIDKEY, "", 0.0, 0, false);
	}

	if (ExecuteQuery() == false) {
		return make_tuple(INVALIDKEY, "", 0.0, 0, false);
	}

	if (mysql_stmt_fetch(mStmt) != 0) {
		return make_tuple(INVALIDKEY, "", 0.0, 0, false);
	}

	return make_tuple(bindUID, bindNickname, bindCredit, bindPoint, bindState);
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

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateUserConnectionState(const int uid, const bool state)
{
	string query = "UPDATE userinfo SET state = ? WHERE UID = ?";

	if (mysql_stmt_prepare(mStmt, query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "InsertNewUser stmt prepare error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	const int colNum = 2;

	MYSQL_BIND binds[colNum];
	memset(binds, 0, sizeof(binds));

	binds[0].buffer_type = MYSQL_TYPE_LONG;
	binds[0].buffer = (void*)&state;

	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = (void*)&uid;

	if (mysql_stmt_bind_param(mStmt, binds) != 0) {

#ifdef Test
		std::cout << "UpdateUserConnectionState stmt bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;

	return true;
}

bool DB::UpdateUserNickname(const int uid, const string& nicknameToChange)
{
	string query = "UPDATE userinfo SET nick = ? WHERE UID = ?";

	if (mysql_stmt_prepare(mStmt, query.c_str(), query.length()) != 0) {
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

	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = (void*)&uid;

	if (mysql_stmt_bind_param(mStmt, binds) != 0) {

#ifdef Test
		std::cout << "UpdateUserNickname stmt bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateUserCredit(const int uid, double credit)
{
	string query = "UPDATE userinfo SET credit = ? WHERE UID = ?";

	if (mysql_stmt_prepare(mStmt, query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "UpdateUserCredit stmt prepare error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	const int colNum = 2;

	MYSQL_BIND binds[colNum];
	memset(binds, 0, sizeof(binds));

	binds[0].buffer_type = MYSQL_TYPE_DOUBLE;
	binds[0].buffer = &credit;

	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = (void*)&uid;

	if (mysql_stmt_bind_param(mStmt, binds) != 0) {

#ifdef Test
		std::cout << "UpdateUserCredit stmt bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateUserPoint(const int uid, unsigned int point)
{
	string query = "UPDATE userinfo SET point = ? WHERE UID = ?";

	if (mysql_stmt_prepare(mStmt, query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "UpdateUserPoint stmt prepare error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	const int colNum = 2;

	MYSQL_BIND binds[colNum];
	memset(binds, 0, sizeof(binds));

	binds[0].buffer_type = MYSQL_TYPE_LONG;
	binds[0].buffer = &point;

	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = (void*)&uid;

	if (mysql_stmt_bind_param(mStmt, binds) != 0) {

#ifdef Test
		std::cout << "UpdateUserPoint stmt bind error: " << mysql_stmt_error(mStmt) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

void DB::DisconnectDB()
{
	mysql_stmt_close(mStmt);
	mysql_close(&mConn);
}
