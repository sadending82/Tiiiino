#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <concurrent_queue.h>
#include <mutex>

#include "../../../../../protocol.h"


enum class eCOMMAND_IOCP {
	CMD_ACCEPT, CMD_RECV, CMD_SEND, CMD_SERVER_RECV, CMD_DBSERVER_RECV, //Basic
	CMD_GAME_WAIT, CMD_GAME_START, CMD_GAME_END, CMD_GAME_RESET //Game Cycle
};

enum class eSocketState
{
	ST_FREE,
	ST_ACCEPT,
	ST_INGAME
};

class WSA_OVER_EX {
public:
	WSA_OVER_EX() = default;
	WSA_OVER_EX(eCOMMAND_IOCP cmd, unsigned char bytes, void* msg);
	~WSA_OVER_EX();

	unsigned char* GetBuf() { return mBuf; }
	const eCOMMAND_IOCP GetCmd() const { return mCmd; }
	WSABUF& GetWsaBuf() { return mWsaBuf; }
	WSAOVERLAPPED& GetWsaOver() { return mWsaOver; }

	void SetCmd(const eCOMMAND_IOCP& cmd) { mCmd = cmd; }

private:
	WSAOVERLAPPED mWsaOver;
	eCOMMAND_IOCP mCmd;
	WSABUF mWsaBuf;
	unsigned char mBuf[BUF_SIZE];
};


class Network
{
public:
	Network();
	virtual ~Network();

	void DisConnect();
	void ErrorDisplay(int errNo);
	void PreRecvPacket(unsigned char* remainMsg, int remainBytes);
	void RecvPacket();
	void SendPacket(void* packet, int bytes);
	
	int GetPrevSize()const { return mPrevSize; }
	SOCKET GetSocket() const { return mSocket; }
	std::mutex& GetStateLockRef() { return mStateLock; }
	eSocketState GetSocketState() const { return mSocketState; }
	void SetSocketState(const eSocketState socketState) { mSocketState = socketState; }

	void AcceptSetting(const eSocketState& socketState,const eCOMMAND_IOCP& commandIocp,SOCKET& socket);
protected:
	WSA_OVER_EX mWsaOverEx;

	SOCKET mSocket;
	std::mutex mStateLock;
	eSocketState mSocketState;
	int mPrevSize;

};

