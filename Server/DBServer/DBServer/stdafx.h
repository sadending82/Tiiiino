#pragma once
#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>

#include <thread>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <random>

#include <windows.h>  
#include <locale.h>
#include <cctype>

#define UNICODE  

#define ID string
#define UNIQUEID int
#define GRADE double
#define POINT int
#define STATE bool
#define DEPARTMENT char
#define EQUIPMENT_BITFLAG long long

#define ITEM_CODE int
#define COUPON_CAN_DUPLICATED bool
#define COUPON_USED	bool


constexpr int MAXLOBBY = 10;
constexpr int INVALIDKEY = -1;
constexpr int BUFSIZE = 200;

constexpr int STARTKEY = 1;

constexpr int SALT_LENGTH = 10;

//###############################
//###############################
#define RUN_DB
//###############################
//###############################

#ifdef _DEBUG
#define DEBUGMSGNOPARAM(args) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args))
#define DEBUGMSGONEPARAM(args,k) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args,k))
#else
#define DEBUGMSGNOPARAM(...)
#define DEBUGMSGONEPARAM(...)
#endif

#include <string>

#include "../../../protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

#ifdef RUN_DB
#include <sqlext.h>  
#include "mysql.h"
#pragma comment(lib, "libmysql.lib")
#endif 
