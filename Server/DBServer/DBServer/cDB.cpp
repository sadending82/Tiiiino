#include "cDB.h"

bool DB::ConnectDB()
{
	mDriver = sql::mysql::get_mysql_driver_instance();
	mConn = mDriver->connect(mHost, mUser, mPW);
	mConn->setSchema(mDBName);

    if (mConn->isValid()) {
        // 연결 성공
        std::cout << "Connected to MySQL server." << std::endl;
    }
    else {
        // 연결 실패
        std::cout << "Failed to connect to MySQL server." << std::endl;
    }

	return false;
}
