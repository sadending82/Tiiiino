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

#define UNICODE  
#include <sqlext.h>  
#include <string>

#include "../../../protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;

<<<<<<< Updated upstream
#include <mysql.h>
#pragma comment(lib, "libmysql.lib")
=======
#include "mysql.h"
#pragma comment(lib, "libmysql.lib")

constexpr int MAXLOBBY = 10;
constexpr int INVALIDKEY = -1;
constexpr int BUFSIZE = 200;

constexpr int STARTKEY = 1;

constexpr int SALT_LENGTH = 10;
>>>>>>> Stashed changes
