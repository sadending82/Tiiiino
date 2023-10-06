#pragma once
#include "cDB.h"

#define Test

bool DB::ConnectDB()
{
	if (mysql_init(GetmConn()) == NULL) {
#ifdef Test
		std::cout << "Mysql Handle Init Fail" << std::endl;
#endif 

		return false;
	}

	if (mysql_real_connect(GetmConn(), GetHost(), GetUser(), GetPassWord(), GetDBName(), GetPort(), (const char*)NULL, 0) != 0) {
#ifdef Test
		std::cout << "DataBase Connect Success" << std::endl;
		mysql_query(GetmConn(), "set names euckr");
#endif
	}
	else {
#ifdef Test
		std::cout << "DataBase Connect Fail" << std::endl;
#endif
		return false;
	}

	if (mysql_select_db(GetmConn(), GetDBName())) {
#ifdef Test
		std::cout << "Select DataBase Fail" << std::endl;
#endif
		return false;
	}

	SetmStmt(mysql_stmt_init(GetmConn()));


	return true;
}

bool DB::ExecuteQuery()
{
	int state = mysql_stmt_execute(GetmStmt());
	if (state != 0) {
#ifdef Test
		std::cout << "Query Execute error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	return true;
}

tuple<string, string, double, int> DB::SelectUserData(const int uid)
{
	string query = "SELECT id, nick, grade, point FROM userinfo WHERE uid = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return tuple<string, string, double, int>();
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));
	paramBind.buffer_type = MYSQL_TYPE_LONG;
	paramBind.buffer = (void*)&uid;

	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt param bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return tuple<string, string, double, int>();
	}

	const int resColNum = 4;
	MYSQL_BIND resultBinds[resColNum];
	memset(resultBinds, 0, sizeof(resultBinds));
	char bindID[MAX_NAME_SIZE];
	char bindNickname[MAX_NAME_SIZE];
	double bindgrade;
	int bindPoint;
	{
		resultBinds[0].buffer_type = MYSQL_TYPE_STRING;
		resultBinds[0].buffer_length = sizeof(bindID);
		resultBinds[0].buffer = bindID;

		resultBinds[1].buffer_type = MYSQL_TYPE_STRING;
		resultBinds[1].buffer_length = sizeof(bindNickname);
		resultBinds[1].buffer = bindNickname;

		resultBinds[2].buffer_type = MYSQL_TYPE_DOUBLE;
		resultBinds[2].buffer = &bindgrade;

		resultBinds[3].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[3].buffer = &bindPoint;

	}

	if (mysql_stmt_bind_result(GetmStmt(), resultBinds) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt result bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return tuple<string, string, double, int>();
	}

	if (ExecuteQuery() == false) {
		return tuple<string, string, double, int>();
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
		return tuple<string, string, double, int>();
	}

	return make_tuple(bindID, bindNickname, bindgrade, bindPoint);
}

tuple<int, string, double, int, bool> DB::SelectUserDataForLogin(const string& id)
{
	string query = "SELECT UID, nick, grade, point, state FROM tiiiino.userinfo WHERE id = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return tuple<int, string, double, int, bool>();
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));

	paramBind.buffer_type = MYSQL_TYPE_STRING;
	paramBind.buffer = (void*)id.c_str();
	paramBind.buffer_length = id.length();


	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt param bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return tuple<int, string, double, int, bool>();
	}

	const int resColNum = 5;
	MYSQL_BIND resultBinds[resColNum];
	memset(resultBinds, 0, sizeof(resultBinds));
	int bindUID, bindPoint;
	char bindNickname[MAX_NAME_SIZE];
	double bindgrade;
	int bindState;
	{
		resultBinds[0].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[0].buffer = &bindUID;

		resultBinds[1].buffer_type = MYSQL_TYPE_STRING;
		resultBinds[1].buffer_length = sizeof(bindNickname);
		resultBinds[1].buffer = bindNickname;

		resultBinds[2].buffer_type = MYSQL_TYPE_DOUBLE;
		resultBinds[2].buffer = &bindgrade;

		resultBinds[3].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[3].buffer = &bindPoint;

		resultBinds[4].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[4].buffer = &bindState;
	}

	if (mysql_stmt_bind_result(GetmStmt(), resultBinds) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt result bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return tuple<int, string, double, int, bool>();
	}

	if (ExecuteQuery() == false) {
		return tuple<int, string, double, int, bool>();
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
		return tuple<int, string, double, int, bool>();
	}

	return make_tuple(bindUID, bindNickname, bindgrade, bindPoint, bindState);
}

vector<string> DB::SelectHash(const string& id)
{
	vector<string> result;
	string query = "SELECT hashedPassword, salt FROM account WHERE ID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "SelectHash stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return vector<string>();
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));

	paramBind.buffer_type = MYSQL_TYPE_STRING;
	paramBind.buffer = (void*)id.c_str();
	paramBind.buffer_length = id.length();


	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
#ifdef Test
		std::cout << "SelectHash stmt param bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return vector<string>();
	}

	const int resColNum = 2;
	MYSQL_BIND resultBinds[resColNum];
	memset(resultBinds, 0, sizeof(resultBinds));
	char bindHash[257];
	char bindSalt[45];
	{
		resultBinds[0].buffer_type = MYSQL_TYPE_STRING;
		resultBinds[0].buffer_length = sizeof(bindHash);
		resultBinds[0].buffer = bindHash;

		resultBinds[1].buffer_type = MYSQL_TYPE_STRING;
		resultBinds[1].buffer_length = sizeof(bindSalt);
		resultBinds[1].buffer = bindSalt;
	}

	if (mysql_stmt_bind_result(GetmStmt(), resultBinds) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt result bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return vector<string>();
	}

	if (ExecuteQuery() == false) {
		return vector<string>();
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
#ifdef Test
		std::cout << "SelectUserData stmt result fetch error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return vector<string>();
	}

	result.push_back(bindHash);
	result.push_back(bindSalt);

	return result;
}

bool DB::InsertNewUser(const string& id)
{
	string query = "INSERT INTO userinfo (ID) VALUES (?)";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "InsertNewUser stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}


	MYSQL_BIND bind;
	memset(&bind, 0, sizeof(bind));

	bind.buffer_type = MYSQL_TYPE_STRING;
	bind.buffer = (void*)id.c_str();
	bind.buffer_length = id.length();

	if (mysql_stmt_bind_param(GetmStmt(), &bind) != 0) {

#ifdef Test
		std::cout << "InsertNewUser stmt bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::InsertNewAccount(const string& id, const string& password)
{
	string query = "INSERT INTO account (ID, hashedPassword, salt) VALUES (?, ?, ?)";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "InsertNewUser stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	string salt = mSecurity->GenerateSalt();
	string hash = mSecurity->HashingPassword(password, salt);

	MYSQL_BIND binds[3];
	memset(binds, 0, sizeof(binds));

	{
		binds[0].buffer_type = MYSQL_TYPE_STRING;
		binds[0].buffer = (void*)id.c_str();
		binds[0].buffer_length = id.length();

		binds[1].buffer_type = MYSQL_TYPE_STRING;
		binds[1].buffer = (void*)hash.c_str();
		binds[1].buffer_length = hash.length();

		binds[2].buffer_type = MYSQL_TYPE_STRING;
		binds[2].buffer = (void*)salt.c_str();
		binds[2].buffer_length = salt.length();
	}

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {

#ifdef Test
		std::cout << "InsertNewUser stmt bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateUserConnectionState(const int uid, const int state)
{
	string query = "UPDATE userinfo SET state = ? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "InsertNewUser stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
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

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {

#ifdef Test
		std::cout << "UpdateUserConnectionState stmt bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateUserNickname(const int uid, const string& nicknameToChange)
{
	string query = "UPDATE userinfo SET nick = ? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "InsertNewUser stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
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

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {

#ifdef Test
		std::cout << "UpdateUserNickname stmt bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateUsergrade(const int uid, double grade)
{
	string query = "UPDATE userinfo SET grade = ? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "UpdateUsergrade stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	const int colNum = 2;

	MYSQL_BIND binds[colNum];
	memset(binds, 0, sizeof(binds));

	binds[0].buffer_type = MYSQL_TYPE_DOUBLE;
	binds[0].buffer = &grade;

	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = (void*)&uid;

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {

#ifdef Test
		std::cout << "UpdateUsergrade stmt bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
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

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "UpdateUserPoint stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
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

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {

#ifdef Test
		std::cout << "UpdateUserPoint stmt bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateUserDepartment(const int uid, const char department)
{
	string query = "UPDATE userinfo SET department = ? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "UpdateUserPoint stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	const int colNum = 2;

	MYSQL_BIND binds[colNum];
	memset(binds, 0, sizeof(binds));

	binds[0].buffer_type = MYSQL_TYPE_TINY;
	binds[0].buffer = (void*)&department;

	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = (void*)&uid;

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {

#ifdef Test
		std::cout << "UpdateUserPoint stmt bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::DeleteAccount(const string& id)
{
	string query = "Delete FROM account WHERE ID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
#ifdef Test
		std::cout << "Delete Account stmt prepare error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	MYSQL_BIND bind;
	memset(&bind, 0, sizeof(bind));

	bind.buffer_type = MYSQL_TYPE_STRING;
	bind.buffer = (void*)id.c_str();
	bind.buffer_length = id.length();

	if (mysql_stmt_bind_param(GetmStmt(), &bind) != 0) {

#ifdef Test
		std::cout << "Delete Account stmt bind error: " << mysql_stmt_error(GetmStmt()) << std::endl;
#endif
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::SignUpNewPlayer(const string& id, const string& password)
{
	bool res = InsertNewAccount(id, password);
	if (res == false) return false;

	res = InsertNewUser(id);
	if (res == false) {
		DeleteAccount(id);
		return false;
	}

	return true;
}

bool DB::CheckVerifyUser(const string& id, const string& password)
{
	vector<string> selectRes = SelectHash(id);

	if (selectRes.empty()) {
		return false;
	}

	string hash = selectRes[0];
	string salt = selectRes[1];

	return GetmSecurity()->VerifyPassword(password, hash, salt);
}

void DB::DisconnectDB()
{
	mysql_stmt_close(GetmStmt());
	mysql_close(GetmConn());
}
