#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <winsock.h>
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <queue>
#include <array>

#include <windows.h>		// Header File For Windows
#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <atomic>
#include <memory>
//#include <gl\glaux.h>		// Header File For The Glaux Library

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

using namespace std;
using namespace chrono;

const static int MAX_TEST = 14;//1000;
const static int START_CLIENT = 7;
const static int MAX_CLIENTS = 14;// MAX_TEST * 2;
const static int INVALID_ID = -1;
const static int MAX_PACKET_SIZE = 255;
const static int MAX_BUFF_SIZE = 255;
constexpr const char* GAMEVERSION = "2.2.0";
constexpr const char* SV_ADDR = "127.0.0.1";
#pragma comment (lib, "ws2_32.lib")

#include "../../protocol.h"

#ifdef _DEBUG
#define DEBUGMSGNOPARAM(args) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args))
#define DEBUGMSGONEPARAM(args,k) (printf("file: %s, line:%d ",__FILE__,__LINE__), printf(args,k))
#else
#define DEBUGMSGNOPARAM(...)
#define DEBUGMSGONEPARAM(...)
#endif

enum OPTYPE { OP_SEND, OP_LOBBY_RECV, OP_GAME_RECV, OP_EVENT };

enum eEVENT_TYPE
{
	EV_MOVE, EV_STOP
};

struct OverlappedEx {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	OPTYPE event_type;
	int event_target;
};

struct CLIENT {
	int id;
	int x = 0;
	int y = 0;
	int z = 0;
	atomic_bool connected;
	atomic_bool bCanMove;

	int mUID;
	int roomSyncID;
	char	hashs[MAX_NAME_SIZE];

	SOCKET client_socket;
	OverlappedEx recv_over;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	int prev_packet_data;
	int curr_packet_size;

	SOCKET l_client_socket;
	OverlappedEx l_recv_over;
	unsigned char l_packet_buf[MAX_PACKET_SIZE];
	int l_prev_packet_data;
	int l_curr_packet_size;
	high_resolution_clock::time_point last_move_time;
};