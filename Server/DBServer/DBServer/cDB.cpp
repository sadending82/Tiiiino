#pragma once
#include "cDB.h"

#ifdef RUN_DB

bool DB::ConnectDB()
{
	if (mysql_init(GetmConn()) == NULL) {

		DEBUGMSGNOPARAM("Mysql Handle Init Fail\n");

		return false;
	}

	if (mysql_real_connect(GetmConn(), GetHost(), GetUser(), GetPassWord(), GetDBName(), GetPort(), (const char*)NULL, 0) != 0) {
		cout << "DataBase Connect Success\n";
		mysql_query(GetmConn(), "set names euckr");
	}
	else {
		DEBUGMSGNOPARAM("DataBase Connect Fail\n");
		return false;
	}

	if (mysql_select_db(GetmConn(), GetDBName())) {
		DEBUGMSGNOPARAM("Select DataBase Fail\n");
		return false;
	}

	SetmStmt(mysql_stmt_init(GetmConn()));


	return true;
}

bool DB::ExecuteQuery()
{
	int state = mysql_stmt_execute(GetmStmt());
	if (state != 0) {
		DEBUGMSGONEPARAM("Query Execute error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	return true;
}

tuple<ID, GRADE, POINT> DB::SelectUserData(const int uid)
{
	string query = "SELECT id, grade, point FROM userinfo WHERE uid = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("SelectUserData stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return tuple<ID, GRADE, POINT>();
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));
	paramBind.buffer_type = MYSQL_TYPE_LONG;
	paramBind.buffer = (void*)&uid;

	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
		DEBUGMSGONEPARAM("SelectUserData stmt param bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return tuple<ID, GRADE, POINT>();
	}

	const int resColNum = 3;
	MYSQL_BIND resultBinds[resColNum];
	memset(resultBinds, 0, sizeof(resultBinds));
	char bindID[MAX_NAME_SIZE];
	double bindgrade;
	int bindPoint;
	{
		resultBinds[0].buffer_type = MYSQL_TYPE_STRING;
		resultBinds[0].buffer_length = sizeof(bindID);
		resultBinds[0].buffer = bindID;

		resultBinds[1].buffer_type = MYSQL_TYPE_DOUBLE;
		resultBinds[1].buffer = &bindgrade;

		resultBinds[2].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[2].buffer = &bindPoint;

	}

	if (mysql_stmt_bind_result(GetmStmt(), resultBinds) != 0) {
		DEBUGMSGONEPARAM("SelectUserData stmt result bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return tuple<ID, GRADE, POINT>();
	}

	if (ExecuteQuery() == false) {
		return tuple<ID, GRADE, POINT>();
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
		return tuple<ID, GRADE, POINT>();
	}

	return make_tuple(bindID, bindgrade, bindPoint);
}

tuple<UNIQUEID, GRADE, POINT, STATE, DEPARTMENT, EQUIPMENT_BITFLAG> DB::SelectUserDataForLogin(const string& id)
{
	string query = "SELECT UID, grade, point, state, department, equippedItems FROM tiiiino.userinfo WHERE id = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("SelectUserData stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return tuple<UNIQUEID, GRADE, POINT, STATE, DEPARTMENT, EQUIPMENT_BITFLAG>();
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));

	paramBind.buffer_type = MYSQL_TYPE_STRING;
	paramBind.buffer = (void*)id.c_str();
	paramBind.buffer_length = id.length();


	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
		DEBUGMSGONEPARAM("SelectUserData stmt param bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return tuple<UNIQUEID, GRADE, POINT, STATE, DEPARTMENT, EQUIPMENT_BITFLAG>();
	}

	const int resColNum = 6;
	MYSQL_BIND resultBinds[resColNum];
	memset(resultBinds, 0, sizeof(resultBinds));
	int bindUID, bindPoint;
	double bindGrade;
	int bindState;
	int bindDepartment;
	long long bindEquippedItems;
	{
		resultBinds[0].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[0].buffer = &bindUID;

		resultBinds[1].buffer_type = MYSQL_TYPE_DOUBLE;
		resultBinds[1].buffer = &bindGrade;

		resultBinds[2].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[2].buffer = &bindPoint;

		resultBinds[3].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[3].buffer = &bindState;

		resultBinds[4].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[4].buffer = &bindDepartment;

		resultBinds[5].buffer_type = MYSQL_TYPE_LONGLONG;
		resultBinds[5].buffer = &bindEquippedItems;
	}

	if (mysql_stmt_bind_result(GetmStmt(), resultBinds) != 0) {
		DEBUGMSGONEPARAM("SelectUserData stmt result bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return tuple<UNIQUEID, GRADE, POINT, STATE, DEPARTMENT, EQUIPMENT_BITFLAG>();
	}

	if (ExecuteQuery() == false) {
		return tuple<UNIQUEID, GRADE, POINT, STATE, DEPARTMENT, EQUIPMENT_BITFLAG>();
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
		return tuple<UNIQUEID, GRADE, POINT, STATE, DEPARTMENT, EQUIPMENT_BITFLAG>();
	}

	return make_tuple(bindUID, bindGrade, bindPoint
		, bindState, static_cast<char>(bindDepartment), bindEquippedItems);
}

vector<string> DB::SelectHash(const string& id)
{
	vector<string> result;
	string query = "SELECT hashedPassword, salt FROM account WHERE ID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("SelectHash stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return vector<string>();
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));

	paramBind.buffer_type = MYSQL_TYPE_STRING;
	paramBind.buffer = (void*)id.c_str();
	paramBind.buffer_length = id.length();


	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
		DEBUGMSGONEPARAM("SelectHash stmt param bind error: %s\n", mysql_stmt_error(GetmStmt()));
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
		DEBUGMSGONEPARAM("SelectUserData stmt result bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return vector<string>();
	}

	if (ExecuteQuery() == false) {
		return vector<string>();
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
		DEBUGMSGONEPARAM("SelectUserData stmt result fetch error: %s\n", mysql_stmt_error(GetmStmt()));
		return vector<string>();
	}

	result.push_back(bindHash);
	result.push_back(bindSalt);

	return result;
}

tuple<GRADE, DEPARTMENT>  DB::SelectUserGradeAndDepartment(const int uid)
{
	string query = "SELECT grade, department FROM userinfo WHERE uid = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("SelectUserData stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return tuple<GRADE, DEPARTMENT>();
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));
	paramBind.buffer_type = MYSQL_TYPE_LONG;
	paramBind.buffer = (void*)&uid;

	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
		DEBUGMSGONEPARAM("SelectUserData stmt param bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return tuple<GRADE, DEPARTMENT>();
	}

	const int resColNum = 2;
	MYSQL_BIND resultBinds[resColNum];
	memset(resultBinds, 0, sizeof(resultBinds));
	int bindDepartment;
	double bindGrade;
	{
		resultBinds[0].buffer_type = MYSQL_TYPE_DOUBLE;
		resultBinds[0].buffer = &bindGrade;

		resultBinds[1].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[1].buffer = &bindDepartment;
	}

	if (mysql_stmt_bind_result(GetmStmt(), resultBinds) != 0) {
		DEBUGMSGONEPARAM("SelectUserData stmt result bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return tuple<GRADE, DEPARTMENT>();
	}

	if (ExecuteQuery() == false) {
		return tuple<GRADE, DEPARTMENT>();
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
		return tuple<GRADE, DEPARTMENT>();
	}

	return make_tuple(bindGrade, static_cast<char>(bindDepartment));
}

long long DB::SelectInventory(const int uid)
{
	string query = "SELECT items FROM inventory WHERE uid = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("Select Inventory stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return 0;
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));
	paramBind.buffer_type = MYSQL_TYPE_LONG;
	paramBind.buffer = (void*)& uid;

	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
		DEBUGMSGONEPARAM("Select Inventory stmt param bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return 0;
	}

	MYSQL_BIND resultBind;
	memset(&resultBind, 0, sizeof(resultBind));
	long long bindItems = 0;
	resultBind.buffer_type = MYSQL_TYPE_LONGLONG;
	resultBind.buffer = &bindItems;

	if (mysql_stmt_bind_result(GetmStmt(), &resultBind) != 0) {
		DEBUGMSGONEPARAM("Select Inventory stmt result bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return 0;
	}

	if (ExecuteQuery() == false) {
		return 0;
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
		return 0;
	}

	return bindItems;
}

int DB::SelectPoint(const int uid)
{
	string query = "SELECT point FROM userinfo WHERE uid = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("Select Point stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return -1;
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));
	paramBind.buffer_type = MYSQL_TYPE_LONG;
	paramBind.buffer = (void*)&uid;

	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
		DEBUGMSGONEPARAM("Select Point stmt param bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return -1;
	}

	MYSQL_BIND resultBind;
	memset(&resultBind, 0, sizeof(resultBind));
	int bindPoint = 0;
	resultBind.buffer_type = MYSQL_TYPE_LONG;
	resultBind.buffer = &bindPoint;

	if (mysql_stmt_bind_result(GetmStmt(), &resultBind) != 0) {
		DEBUGMSGONEPARAM("Select Point stmt result bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return -1;
	}

	if (ExecuteQuery() == false) {
		return -1;
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
		DEBUGMSGONEPARAM("Select Point stmt result fetch error: %s\n", mysql_stmt_error(GetmStmt()));
		return -1;
	}

	return bindPoint;
}

vector<rankInfo> DB::SelectRanking()
{
	vector<rankInfo> rankList;
	string query = "SELECT department, point FROM department_rank ORDER BY point DESC LIMIT 10";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("SelectRanking stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return vector<rankInfo>();
	}

	const int resColNum = 2;
	MYSQL_BIND resultBinds[resColNum];
	memset(resultBinds, 0, sizeof(resultBinds));
	int bindDepartment;
	int bindPoint;
	{
		resultBinds[0].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[0].buffer = &bindDepartment;

		resultBinds[1].buffer_type = MYSQL_TYPE_LONG;
		resultBinds[1].buffer = &bindPoint;
	}

	if (mysql_stmt_bind_result(GetmStmt(), resultBinds) != 0) {
		DEBUGMSGONEPARAM("SelectRanking stmt result bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return vector<rankInfo>();
	}

	if (ExecuteQuery() == false) {
		return vector<rankInfo>();
	}

	while (1) {
		int status = mysql_stmt_fetch(GetmStmt());
		if (status == 1 || status == MYSQL_NO_DATA) break;
		rankInfo rInfo;
		rInfo.department = static_cast<char>(bindDepartment);
		rInfo.point = bindPoint;
		rankList.push_back(rInfo);
	}

	return rankList;
}

tuple<ITEM_CODE, COUPON_CAN_DUPLICATED, COUPON_USED> DB::SelectCouponInfo(const string& code)
{
	string query = "SELECT itemCode, CanDuplicated, used FROM coupons WHERE code = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("Select Coupon Info stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return make_tuple(INVALIDKEY, 0, 0);
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));
	paramBind.buffer_type = MYSQL_TYPE_STRING;
	paramBind.buffer = (void*)code.c_str();
	paramBind.buffer_length = code.length();

	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
		DEBUGMSGONEPARAM("Select Coupon Info stmt param bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return make_tuple(INVALIDKEY, 0, 0);
	}

	const int resColNum = 3;
	MYSQL_BIND resultBind[resColNum];
	memset(&resultBind, 0, sizeof(resultBind));
	int bindItemCode = 0;
	int bindCanDuplicated = false;
	int bindUsed = false;
	resultBind[0].buffer_type = MYSQL_TYPE_LONG;
	resultBind[0].buffer = &bindItemCode;
	resultBind[1].buffer_type = MYSQL_TYPE_LONG;
	resultBind[1].buffer = &bindCanDuplicated;
	resultBind[2].buffer_type = MYSQL_TYPE_LONG;
	resultBind[2].buffer = &bindUsed;

	if (mysql_stmt_bind_result(GetmStmt(), resultBind) != 0) {
		DEBUGMSGONEPARAM("Select Coupon Info stmt result bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return make_tuple(INVALIDKEY, 0, 0);
	}

	if (ExecuteQuery() == false) {
		return make_tuple(INVALIDKEY, 0, 0);
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
		DEBUGMSGONEPARAM("Select Coupon Info stmt result fetch error: %s\n", mysql_stmt_error(GetmStmt()));
		return make_tuple(INVALIDKEY, 0, 0);
	}

	return make_tuple(bindItemCode, bindCanDuplicated, bindUsed);
}

long long DB::SelectEquipmentItems(const int uid)
{
	string query = "SELECT equippedItems FROM userinfo WHERE uid = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("Select equippedItemsstmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return 0;
	}

	MYSQL_BIND paramBind;
	memset(&paramBind, 0, sizeof(paramBind));
	paramBind.buffer_type = MYSQL_TYPE_LONG;
	paramBind.buffer = (void*)&uid;

	if (mysql_stmt_bind_param(GetmStmt(), &paramBind) != 0) {
		DEBUGMSGONEPARAM("Select equippedItems stmt param bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return 0;
	}

	MYSQL_BIND resultBind;
	memset(&resultBind, 0, sizeof(resultBind));
	long long bindItems = 0;
	resultBind.buffer_type = MYSQL_TYPE_LONGLONG;
	resultBind.buffer = &bindItems;

	if (mysql_stmt_bind_result(GetmStmt(), &resultBind) != 0) {
		DEBUGMSGONEPARAM("Select equippedItems stmt result bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return 0;
	}

	if (ExecuteQuery() == false) {
		return 0;
	}

	if (mysql_stmt_fetch(GetmStmt()) != 0) {
		return 0;
	}

	return bindItems;
}

bool DB::InsertNewUser(const string& id, const char department)
{
	string query = "INSERT INTO userinfo (ID, department) VALUES (?, ?)";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("InsertNewUser stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}


	MYSQL_BIND bind[2];
	memset(&bind, 0, sizeof(bind));

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = (void*)id.c_str();
	bind[0].buffer_length = id.length();

	bind[1].buffer_type = MYSQL_TYPE_TINY;
	bind[1].buffer = (void*)&department;

	if (mysql_stmt_bind_param(GetmStmt(), bind) != 0) {
		DEBUGMSGONEPARAM("InsertNewUser stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
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
		DEBUGMSGONEPARAM("InsertNewUser stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
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
		DEBUGMSGONEPARAM("InsertNewUser stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::InsertNewInventory(const int uid)
{

	return false;
}

bool DB::UpdateUserConnectionState(const int uid, const int state)
{
	string query = "UPDATE userinfo SET state = ? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("InsertNewUser stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
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
		DEBUGMSGONEPARAM("UpdateUserConnectionState stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateUserGrade(const int uid, double grade)
{
	string query = "UPDATE userinfo SET grade = ? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("UpdateUsergrade stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
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
		DEBUGMSGONEPARAM("UpdateUsergrade stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateUserPoint(const int uid, unsigned int point)
{
	string query = "UPDATE userinfo SET point = point + ? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("UpdateUserPoint stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
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
		DEBUGMSGONEPARAM("UpdateUserPoint stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
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
		DEBUGMSGONEPARAM("UpdateUserPoint stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
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
		DEBUGMSGONEPARAM("UpdateUserPoint stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateInventory(const int uid, const int itemCode)
{
	string query = "INSERT INTO inventory (uid, items) VALUES (?, ?) ON DUPLICATE KEY UPDATE items = items | ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("UpdateInventory stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	const int num = 3;

	uint64_t bit = 1ULL << itemCode;

	MYSQL_BIND binds[num];
	memset(binds, 0, sizeof(binds));

	binds[0].buffer_type = MYSQL_TYPE_LONG;
	binds[0].buffer = (void*)&uid;

	binds[1].buffer_type = MYSQL_TYPE_LONGLONG;
	binds[1].buffer = (void*)&bit;

	binds[2].buffer_type = MYSQL_TYPE_LONGLONG;
	binds[2].buffer = (void*)&bit;

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {
		DEBUGMSGONEPARAM("UpdateInventory stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateInventoryDeleteItem(const int uid, const int itemCode)
{
	string query = "UPDATE inventory SET items = items & ~? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("UpdateUnequipItem stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	const int num = 2;

	uint64_t bit = 1ULL << itemCode;

	MYSQL_BIND binds[num];
	memset(binds, 0, sizeof(binds));


	binds[0].buffer_type = MYSQL_TYPE_LONGLONG;
	binds[0].buffer = (void*)&bit;

	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = (void*)&uid;

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {
		DEBUGMSGONEPARAM("UpdateUnequipItem stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateEquipItemFlag(const int uid, long long bitFlag)
{
	string query = "UPDATE userinfo SET equippedItems = ? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("UpdateUnquipItem stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	const int num = 2;

	MYSQL_BIND binds[num];
	memset(binds, 0, sizeof(binds));


	binds[0].buffer_type = MYSQL_TYPE_LONGLONG;
	binds[0].buffer = (void*)&bitFlag;

	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = (void*)&uid;

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {
		DEBUGMSGONEPARAM("UpdateUnquipItem stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::UpdateCouponUsed(const string& code, const bool isUsed)
{
	string query = "UPDATE coupons SET used = ? WHERE code = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("UpdateUnequipItem stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	MYSQL_BIND binds[2];
	memset(bind, 0, sizeof(binds));

	binds[0].buffer_type = MYSQL_TYPE_LONG;
	binds[0].buffer = (void*)&isUsed;

	binds[1].buffer_type = MYSQL_TYPE_STRING;
	binds[1].buffer_length = sizeof(code.c_str());
	binds[1].buffer = (void*)code.c_str();

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {
		DEBUGMSGONEPARAM("UpdateUnequipItem stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
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
		DEBUGMSGONEPARAM("Delete Account stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	MYSQL_BIND bind;
	memset(&bind, 0, sizeof(bind));

	bind.buffer_type = MYSQL_TYPE_STRING;
	bind.buffer = (void*)id.c_str();
	bind.buffer_length = id.length();

	if (mysql_stmt_bind_param(GetmStmt(), &bind) != 0) {
		DEBUGMSGONEPARAM("Delete Account stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::DeleteItemInInventory(const int uid, const int itemCode)
{
	string query = "UPDATE inventory SET items = items & ~? WHERE UID = ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("DeleteItemInInventory stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	const int num = 2;

	uint64_t bit = 1ULL << itemCode;

	MYSQL_BIND binds[num];
	memset(binds, 0, sizeof(binds));


	binds[0].buffer_type = MYSQL_TYPE_LONGLONG;
	binds[0].buffer = (void*)&bit;

	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = (void*)&uid;

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {
		DEBUGMSGONEPARAM("DeleteItemInInventory stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

bool DB::SignUpNewPlayer(const string& id, const string& password, const char department)
{
	bool res = InsertNewAccount(id, password);
	if (res == false) return false;
	res = InsertNewUser(id, department);
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

bool DB::UpdateRanking(const char department, const int incrementPoint)
{
	string query = "INSERT INTO department_rank (department, point) VALUES (?, ?) ON DUPLICATE KEY UPDATE point = point + ?";

	if (mysql_stmt_prepare(GetmStmt(), query.c_str(), query.length()) != 0) {
		DEBUGMSGONEPARAM("UpdateRanking stmt prepare error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	const int num = 3;

	MYSQL_BIND binds[num];
	memset(binds, 0, sizeof(binds));

	binds[0].buffer_type = MYSQL_TYPE_TINY;
	binds[0].buffer = (void*)&department;

	binds[1].buffer_type = MYSQL_TYPE_LONG;
	binds[1].buffer = (void*)&incrementPoint;

	binds[2].buffer_type = MYSQL_TYPE_LONG;
	binds[2].buffer = (void*)&incrementPoint;

	if (mysql_stmt_bind_param(GetmStmt(), binds) != 0) {
		DEBUGMSGONEPARAM("UpdateRanking stmt bind error: %s\n", mysql_stmt_error(GetmStmt()));
		return false;
	}

	if (ExecuteQuery() == false) {
		return false;
	}

	return true;
}

void DB::DisconnectDB()
{
	mysql_stmt_close(GetmStmt());
	mysql_close(GetmConn());
}

void DB::CreateDummyAccount(int count)
{
	for (int i = 0; i < count; ++i) {

		char id[MAX_NAME_SIZE], pw[MAX_NAME_SIZE];
		string dummyID = "dummy" + to_string(i);
		strcpy_s(id, dummyID.c_str());
		sprintf_s(pw, "%d", i);

		InsertNewUser(id, (char)eDepartment::Game);
		InsertNewAccount(id, pw);
	}
}

#endif