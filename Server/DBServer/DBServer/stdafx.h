#pragma once
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
#include <map>

#include <windows.h>  
#include <locale.h>

#define UNICODE  
#include <sqlext.h>  
#include <string>

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <memory>


#include "../../../protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;

constexpr int MAXLOBBY = 10;
constexpr int INVALIDKEY = -1;
constexpr int BUFSIZE = 200;

constexpr int STARTKEY = 1;

#include "mysql_connection.h"

#include <mysql/jdbc.h>