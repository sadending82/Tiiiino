#pragma once
#include "dummy.h"
#include <string>
#include <random>
extern HWND		hWnd;

HANDLE g_hiocp;

high_resolution_clock::time_point last_connect_time;

array<int, MAX_CLIENTS> client_map;
array<CLIENT, MAX_CLIENTS> g_clients;
atomic_int num_connections; //1번째부터 들어옴.
atomic_int client_to_close;
atomic_int active_clients;

int	global_delay;				// ms단위, 1000이 넘으면 클라이언트 증가 종료

vector <thread*> worker_threads;
thread test_thread;
Timer* gTimer;

float point_cloud[MAX_TEST * 2];

// 나중에 NPC까지 추가 확장 용
struct ALIEN {
	int id;
	int x, y;
	int visible_count;
};

void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러" << lpMsgBuf << std::endl;

	MessageBox(hWnd, lpMsgBuf, L"ERROR", 0);
	LocalFree(lpMsgBuf);
	// while (true);
}

void DisconnectClient(int ci)
{
	bool status = true;
	if (true == atomic_compare_exchange_strong(&g_clients[ci].connected, &status, false)) {
		closesocket(g_clients[ci].client_socket);
		active_clients--;
	}
	 cout << "Client [" << ci << "] Disconnected!\n";
}

void SendPacketToLobby(int cl, void* packet)
{
	int psize = reinterpret_cast<unsigned char*>(packet)[0];
	int ptype = reinterpret_cast<unsigned char*>(packet)[1];
	OverlappedEx* over = new OverlappedEx;
	over->event_type = OP_SEND;
	memcpy(over->IOCP_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<CHAR*>(over->IOCP_buf);
	over->wsabuf.len = psize;
	int ret = WSASend(g_clients[cl].l_client_socket, &over->wsabuf, 1, NULL, 0,
		&over->over, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			DEBUGMSGONEPARAM("Error in SendPacket to Lobby // ci: %d\n", cl);
	}
	// std::cout << "Send Packet [" << ptype << "] To Client : " << cl << std::endl;
}

void SendPacketToGame(int cl, void* packet)
{
	int psize = reinterpret_cast<unsigned char*>(packet)[0];
	int ptype = reinterpret_cast<unsigned char*>(packet)[1];
	OverlappedEx* over = new OverlappedEx;
	over->event_type = OP_SEND;
	memcpy(over->IOCP_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<CHAR*>(over->IOCP_buf);
	over->wsabuf.len = psize;
	int ret = WSASend(g_clients[cl].client_socket, &over->wsabuf, 1, NULL, 0,
		&over->over, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			DEBUGMSGONEPARAM("Error in SendPacket to Game // ci: %d\n", cl);
	}
	// std::cout << "Send Packet [" << ptype << "] To Client : " << cl << std::endl;
}

void Game_ProcessPacket(int ci, unsigned char packet[])
{
	switch (packet[1]) {
	case SC_LOGIN_OK:
	{
		SC_LOGIN_OK_PACKET* p = reinterpret_cast<SC_LOGIN_OK_PACKET*>(packet);
		g_clients[ci].roomSyncID = p->id;
		g_clients[ci].x = 240.f - ((p->id % 4) * 160);
		g_clients[ci].y = 0 - (p->id / 4 * 160);
		g_clients[ci].z = 0;
		break;
	}
	case SC_GAME_START: {
		g_clients[ci].bCanMove = true;
		break;
	}
	case SC_GAME_PLAYERLOAD_OK:{
		SC_GAME_PLAYERLOAD_OK_PACKET* p = reinterpret_cast<SC_GAME_PLAYERLOAD_OK_PACKET*>(packet);
		SendGameLoadAck(ci);
		break;
	}
	case SC_ADD_PLAYER: {
		SC_ADD_PLAYER_PACKET* p = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(packet);
		if (g_clients[ci].roomSyncID == p->id) {
			/*g_clients[ci].x = p->x;
			g_clients[ci].y = p->y;
			g_clients[ci].z = p->z;*/
		}

		break;
	}
	case SC_MOVE_PLAYER: {
		SC_MOVE_PLAYER_PACKET* move_packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(packet);
		if (move_packet->id < MAX_CLIENTS) {
			int my_id = client_map[ci];
			if (-1 != my_id) {
				
			}
			if (ci == my_id) {
				if (0 != move_packet->move_time) {
					auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - move_packet->move_time;

					if (global_delay < d_ms) global_delay++;
					else if (global_delay > d_ms) global_delay--;
				}
			}
		}
		break;
	}
	case SC_PING: {
		SC_PING_PACKET* p= reinterpret_cast<SC_PING_PACKET*>(packet);
		SendPingPacket(ci, p->ping);

		break;
	}
	case SC_GAME_END: {
		g_clients[ci].bCanMove = false;
		break;
	}
	default: {
		break;
	}
	}
}

void Lobby_ProcessPacket(int ci, unsigned char packet[]) {
	switch (packet[1]) {
	case LC_LOGIN_OK: {
		g_clients[ci].connected = true;
		active_clients++;
		LC_LOGIN_OK_PACKET* login_packet = reinterpret_cast<LC_LOGIN_OK_PACKET*>(packet);
		int my_id = ci;
		client_map[ci] = my_id;
		g_clients[ci].mUID = login_packet->UID;
		DEBUGMSGONEPARAM("login ok // UID: %d\n", login_packet->UID);
		SendMatchingPacket(ci);

		break;
	}
	case LC_MATCH_RESPONSE: {
		LC_MATCH_RESPONSE_PACKET* p = reinterpret_cast<LC_MATCH_RESPONSE_PACKET*>(packet);
		strcpy_s(g_clients[ci].hashs, sizeof(g_clients[ci].hashs), p->hashs);
		ConnectGameServer(ci);
		SendGameServerLoginPacket(ci);
		//SendGameLoadAck(ci);
		break;
	}

	default: {
		break;
	}
	}
}

void Worker_Thread()
{
	while (true) {
		DWORD io_size;
		unsigned long long ci;
		OverlappedEx* over;
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, &ci,
			reinterpret_cast<LPWSAOVERLAPPED*>(&over), INFINITE);
		// std::cout << "GQCS :";
		int client_id = static_cast<int>(ci);
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			if (64 == err_no) DisconnectClient(client_id);
			else {
				// error_display("GQCS : ", WSAGetLastError());
				DisconnectClient(client_id);
			}
			if (OP_SEND == over->event_type) delete over;
		}
		if (0 == io_size) {
			DisconnectClient(client_id);
			continue;
		}

		switch (over->event_type) {
		case OP_LOBBY_RECV: {
			//std::cout << "RECV from Client :" << ci;
				//std::cout << "  IO_SIZE : " << io_size << std::endl;
			unsigned char* buf = g_clients[ci].l_recv_over.IOCP_buf;
			unsigned psize = g_clients[ci].l_curr_packet_size;
			unsigned pr_size = g_clients[ci].l_prev_packet_data;
			while (io_size > 0) {
				if (0 == psize) psize = buf[0];
				if (io_size + pr_size >= psize) {
					// 지금 패킷 완성 가능
					unsigned char packet[MAX_PACKET_SIZE];
					memcpy(packet, g_clients[ci].l_packet_buf, pr_size);
					memcpy(packet + pr_size, buf, psize - pr_size);
					Lobby_ProcessPacket(static_cast<int>(ci), packet);
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0; pr_size = 0;
				}
				else {
					memcpy(g_clients[ci].l_packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
					break;
				}
			}
			g_clients[ci].l_curr_packet_size = psize;
			g_clients[ci].l_prev_packet_data = pr_size;
			DWORD recv_flag = 0;
			int ret = WSARecv(g_clients[ci].l_client_socket,
				&g_clients[ci].l_recv_over.wsabuf, 1,
				NULL, &recv_flag, &g_clients[ci].l_recv_over.over, NULL);
			if (SOCKET_ERROR == ret) {
				int err_no = WSAGetLastError();
				if (err_no != WSA_IO_PENDING)
				{
					//error_display("RECV ERROR", err_no);
					DisconnectClient(client_id);
				}
			}
			break;
		}
		case OP_GAME_RECV: {
			unsigned char* buf = g_clients[ci].recv_over.IOCP_buf;
			unsigned psize = g_clients[ci].curr_packet_size;
			unsigned pr_size = g_clients[ci].prev_packet_data;
			while (io_size > 0) {
				if (0 == psize) psize = buf[0];
				if (io_size + pr_size >= psize) {
					// 지금 패킷 완성 가능
					unsigned char packet[MAX_PACKET_SIZE];
					memcpy(packet, g_clients[ci].packet_buf, pr_size);
					memcpy(packet + pr_size, buf, psize - pr_size);
					Game_ProcessPacket(static_cast<int>(ci), packet);
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0; pr_size = 0;
				}
				else {
					memcpy(g_clients[ci].packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
					break;
				}
			}
			g_clients[ci].curr_packet_size = psize;
			g_clients[ci].prev_packet_data = pr_size;
			DWORD recv_flag = 0;
			int ret = WSARecv(g_clients[ci].client_socket,
				&g_clients[ci].recv_over.wsabuf, 1,
				NULL, &recv_flag, &g_clients[ci].recv_over.over, NULL);
			if (SOCKET_ERROR == ret) {
				int err_no = WSAGetLastError();
				if (err_no != WSA_IO_PENDING)
				{
					//error_display("RECV ERROR", err_no);
					DisconnectClient(client_id);
				}
			}
			break;
		}
		case OP_SEND: {
			if (io_size != over->wsabuf.len) {
				// std::cout << "Send Incomplete Error!\n";
				DisconnectClient(client_id);
			}
			delete over;
			break;
		}
		case OP_EVENT: {
			// Not Implemented Yet
			delete over;
			break;
		}
		default:{
			std::cout << "Unknown GQCS event!\n";
			while (true);
			break;
		}
		}
	}
}

constexpr int DELAY_LIMIT = 100;
constexpr int DELAY_LIMIT2 = 150;
constexpr int ACCEPT_DELY = 50;

void Adjust_Number_Of_Client()
{
	static int delay_multiplier = 1;
	static int max_limit = MAXINT;
	static bool increasing = true;

	if (active_clients >= MAX_TEST) return;
	if (num_connections >= MAX_CLIENTS) return;

	auto duration = high_resolution_clock::now() - last_connect_time;
	if (ACCEPT_DELY * delay_multiplier > duration_cast<milliseconds>(duration).count()) return;

	int t_delay = global_delay;
	if (DELAY_LIMIT2 < t_delay) {
		if (true == increasing) {
			max_limit = active_clients;
			increasing = false;
		}
		if (100 > active_clients) return;
		if (ACCEPT_DELY * 10 > duration_cast<milliseconds>(duration).count()) return;
		last_connect_time = high_resolution_clock::now();
		DisconnectClient(client_to_close);
		client_to_close++;
		return;
	}
	else
		if (DELAY_LIMIT < t_delay) {
			delay_multiplier = 10;
			return;
		}
	if (max_limit - (max_limit / 20) < active_clients) return;

	increasing = true;
	last_connect_time = high_resolution_clock::now();
	g_clients[num_connections].l_client_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(LOBBYSERVERPORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(g_clients[num_connections].l_client_socket, (sockaddr*)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	if (0 != Result) {
		error_display("WSAConnect : ", GetLastError());
	}

	g_clients[num_connections].l_curr_packet_size = 0;
	g_clients[num_connections].l_prev_packet_data = 0;
	ZeroMemory(&g_clients[num_connections].l_recv_over, sizeof(g_clients[num_connections].l_recv_over));
	g_clients[num_connections].l_recv_over.event_type = OP_LOBBY_RECV;
	g_clients[num_connections].l_recv_over.wsabuf.buf =
		reinterpret_cast<CHAR*>(g_clients[num_connections].l_recv_over.IOCP_buf);
	g_clients[num_connections].l_recv_over.wsabuf.len = sizeof(g_clients[num_connections].l_recv_over.IOCP_buf);

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_clients[num_connections].l_client_socket), g_hiocp, num_connections, 0);

	CL_LOGIN_PACKET sPacket;
	int temp = num_connections;
	sPacket.size = sizeof(sPacket);
	sPacket.type = CL_LOGIN;
	sprintf_s(sPacket.id, "%d", temp);
	sprintf_s(sPacket.password, "%d", temp);
	SendPacketToLobby(num_connections, &sPacket);

	int ret = WSARecv(g_clients[num_connections].l_client_socket, &g_clients[num_connections].l_recv_over.wsabuf, 1,
		NULL, &recv_flag, &g_clients[num_connections].l_recv_over.over, NULL);
	if (SOCKET_ERROR == ret) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING)
		{
			error_display("RECV ERROR", err_no);
			goto fail_to_connect;
		}
	}
	num_connections++;
fail_to_connect:
	return;
}

void ConnectGameServer(int key) 
{
	g_clients[key].client_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(GAMESERVERPORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(g_clients[key].client_socket, (sockaddr*)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	if (0 != Result) {
		error_display("WSAConnect : ", GetLastError());
	}

	g_clients[key].curr_packet_size = 0;
	g_clients[key].prev_packet_data = 0;
	ZeroMemory(&g_clients[key].recv_over, sizeof(g_clients[key].recv_over));
	g_clients[key].recv_over.event_type = OP_GAME_RECV;
	g_clients[key].recv_over.wsabuf.buf =
		reinterpret_cast<CHAR*>(g_clients[key].recv_over.IOCP_buf);
	g_clients[key].recv_over.wsabuf.len = sizeof(g_clients[key].recv_over.IOCP_buf);

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_clients[key].client_socket), g_hiocp, key, 0);

	int ret = WSARecv(g_clients[key].client_socket, &g_clients[key].recv_over.wsabuf, 1,
		NULL, &recv_flag, &g_clients[key].recv_over.over, NULL);
	if (SOCKET_ERROR == ret) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING)
		{
			error_display("RECV ERROR", err_no);
		}
	}
}

void Test_Thread()
{
	while (true) {
		//Sleep(max(20, global_delay));
		Adjust_Number_Of_Client();
		MoveWorker();
	}
}

void MoveWorker()
{
	for (int i = 0; i < num_connections; ++i) {
		if (false == g_clients[i].connected) continue;
		if (false == g_clients[i].bCanMove) continue;
		//60프레임으로 움직임 보내기.
		if (g_clients[i].last_move_time + 0.016s > high_resolution_clock::now()) continue;
		g_clients[i].last_move_time = high_resolution_clock::now();
		SendMoveRand(i);
	}
}

void InitializeNetwork()
{
	for (auto& cl : g_clients) {
		cl.connected = false;
		cl.id = INVALID_ID;
	}

	for (auto& cl : client_map) cl = -1;
	num_connections = 0;
	last_connect_time = high_resolution_clock::now();

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	for (int i = 0; i < 6; ++i)
		worker_threads.push_back(new std::thread{ Worker_Thread });
	//gTimer->Init(g_hiocp);
	test_thread = thread{ Test_Thread };

}

void ShutdownNetwork()
{
	test_thread.join();
	for (auto pth : worker_threads) {
		pth->join();
		delete pth;
	}
}

void Do_Network()
{
	return;
}

void GetPointCloud(int* size, float** points)
{
	int index = 0;
	for (int i = 0; i < num_connections; ++i)
		if (true == g_clients[i].connected) {
			point_cloud[index * 2] = static_cast<float>(g_clients[i].x);
			point_cloud[index * 2 + 1] = static_cast<float>(g_clients[i].y);
			index++;
		}

	*size = index;
	*points = point_cloud;
}

void SendMatchingPacket(int key)
{
	CL_MATCH_PACKET sPacket;
	sPacket.size = sizeof(sPacket);
	sPacket.type = CL_MATCH;
	SendPacketToLobby(key, &sPacket);
}

void SendMoveRand(int key)//
{
	CS_MOVE_PACKET my_packet;
	my_packet.size = sizeof(my_packet);
	my_packet.type = CS_MOVE;

	//g_clients[key].x += 0;//rand() % 10;
	my_packet.x = static_cast<float>(g_clients[key].x);
	//g_clients[key].y += 0;// rand() % 10;
	my_packet.y = static_cast<float>(g_clients[key].y);
	//g_clients[key].y += 0;//rand() % 10;
	my_packet.z = static_cast<float>(g_clients[key].z);

	my_packet.inair = false;
	my_packet.rw = static_cast<float>((rand() %100)) / 100.f;
	my_packet.rx = static_cast<float>((rand() % 100)) / 100.f;
	my_packet.ry = static_cast<float>((rand() % 100)) / 100.f;
	my_packet.rz = static_cast<float>((rand() % 100)) / 100.f;
	my_packet.sx = 0;
	my_packet.sy = 0;
	my_packet.sz = 0;
	

	my_packet.speed = 0;
	my_packet.move_time = static_cast<unsigned>(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count());
	SendPacketToGame(key, &my_packet);
}

void SendGameLoadAck(int key) 
{
	CS_GAME_PLAYERLOAD_ACK_PACKET sPacket;
	sPacket.size = sizeof(sPacket);
	sPacket.type = CS_GAME_PLAYERLOAD_ACK;

	SendPacketToGame(key, &sPacket);
}

void SendGameServerLoginPacket(int key)
{
	CS_LOGIN_PACKET sPacket;
	sPacket.size = sizeof(sPacket);
	sPacket.type = CS_LOGIN;
	sprintf_s(sPacket.name, "%d", key);
	sprintf_s(sPacket.passWord, "%d", key);
	sPacket.uID = g_clients[key].mUID;
	strcpy_s(sPacket.hashs, sizeof(sPacket.hashs), g_clients[key].hashs);
	sPacket.roomID = 0;

	SendPacketToGame(key, &sPacket);
}

void SendPingPacket(int key, const long long ping)
{
	CS_PING_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_PING;
	packet.ping = ping;

	SendPacketToGame(key, &packet);
}