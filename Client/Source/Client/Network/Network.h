// Fill out your copyright notice in the Description page of Project Settings.
// �𸮾� Ŭ�������� ��������� generated.h�Ʒ��� �߰��ϸ� ������ �߻��ϹǷ� ���� �߰��ؾ��մϴ�.
#pragma once

#include <iostream>
#include <memory>
#include <WS2tcpip.h>
#include "../../../../protocol.h"
#include "CoreMinimal.h"







enum class AnimType
{
};

void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_Gamecallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

static std::shared_ptr<class Network> m_Network;
class ATinoCharacter;


void send_login_packet(); 
void send_move_packet(const bool& inair, const float& x, const float& y, const float& z, FQuat& rotate, const float& value, const FVector& speedVec);


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
	~WSA_OVER_EX()
	{

	}

	unsigned char* GetBuf() { return mBuf; }
	WSABUF& GetWsaBuf() { return mWsaBuf; }
	WSAOVERLAPPED& GetWsaOver() { return mWsaOver; }


private:
	WSAOVERLAPPED mWsaOver;
	WSABUF mWsaBuf;
	unsigned char mBuf[BUF_SIZE];
};


class CLIENT_API Network : public std::enable_shared_from_this<Network>
{
public:
	Network();
	~Network();


	const int getNewId();
	void release();
	bool init();
	void process_packet(unsigned char* p);

	static std::shared_ptr<class Network> GetNetwork();


public:
	WSADATA WSAData;
	void error_display(int err_no);


	ATinoCharacter* mMyCharacter;
	//FString MyCharacterName;
	//FString MyCharacterPassWord;
	UPROPERTY()
		ATinoCharacter* mOtherCharacter[MAX_USER];
	int mGeneratedID;
	bool bLevelOpenTriggerEnabled = false;	//��������� ���� �ʱ�ȭ�� �ƴ�, OpenLevel�� ���� �ʱ�ȭ �϶� ������ �ɱ� ����. 
	bool bLoginFlag = false;	//�α��ν� true, Game������ ������ �ٽ� �κ�� ���ƿ� �� 
	//Flag�� true��� �α��� â�� ����� �ʱ� ���ؼ� �ִ� Ʈ��.
	short GameServerPort = -1;		//���Ӽ��� ���ӿ� port

private:
	bool isInit;

public:
	int ClientID;
	bool ConnectServer();
	SOCKET s_socket;
	SOCKADDR_IN server_addr;
	WSA_OVER_EX recv_expover;
	int		_prev_size;
	bool bIsConnected;	//�ӽ� ����
	void RecvPacket() ;
	// 112.152.55.49  127.0.0.1  , 112.153.53.142
	const char* SERVER_ADDR = "127.0.0.1";
};


