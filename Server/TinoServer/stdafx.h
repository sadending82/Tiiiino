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

#include "../../protocol.h"
#include "OverExp.h"
#include "LobbyProtocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;
using namespace std::chrono;