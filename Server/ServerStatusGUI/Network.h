#pragma once
#include "framework.h"
#include "../../protocol.h"

extern class Network* gNetwork; 
void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);

void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

void send_alert_packet(SOCKET& sock, wstring& alert);

class WSA_OVER_EX {
public:
	WSA_OVER_EX() = default;
	WSA_OVER_EX(unsigned char bytes, void* msg)
	{
		ZeroMemory(&mWsaOver, sizeof(mWsaOver));
		mWsaBuf.buf = reinterpret_cast<char*>(mBuf);
		mWsaBuf.len = bytes;
		memcpy(mBuf, msg, bytes);
	}
	~WSA_OVER_EX(){}
	unsigned char* GetBuf() { return mBuf; }
	WSABUF& GetWsaBuf() { return mWsaBuf; }
	WSAOVERLAPPED& GetWsaOver() { return mWsaOver; }

private:
	WSAOVERLAPPED mWsaOver;
	WSABUF mWsaBuf;
	unsigned char mBuf[BUF_SIZE];
};

class Network
{
public:
	Network();
	~Network();
	void release();
	bool init();
	void process_packet(unsigned char* p);
	void error_display(int err_no);
	bool ConnectServer();
	bool RecvPacket();
public:
	WSADATA WSAData{};
	SOCKET s_socket{};
	SOCKADDR_IN server_addr{};
	WSA_OVER_EX recv_expover{};
	int		prev_size{};
	const char* LOBBYSERVER_ADDR = "127.0.0.1";
};

