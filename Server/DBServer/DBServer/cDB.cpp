#include "cDB.h"

bool DB::ConnectDB()
{
	mDriver = sql::mysql::get_mysql_driver_instance();
	mConn = mDriver->connect(mHost, mUser, mPW);
	mConn->setSchema(mDBName);

    if (mConn->isValid()) {
        // ���� ����
        std::cout << "Connected to MySQL server." << std::endl;
    }
    else {
        // ���� ����
        std::cout << "Failed to connect to MySQL server." << std::endl;
    }

	return false;
}
