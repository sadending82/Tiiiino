// Fill out your copyright notice in the Description page of Project Settings.
// �𸮾� Ŭ�������� ��������� generated.h�Ʒ��� �߰��ϸ� ������ �߻��ϹǷ� ���� �߰��ؾ��մϴ�.
#pragma once

#include <iostream>
#include <memory>
#include <WS2tcpip.h>
#include "../../../../protocol.h"
#include "CoreMinimal.h"
#include "GameDataManager/GameDataManager.h"

constexpr const char* GAMEVERSION = "2.2.2";


struct sGameResult {
	int rank;
	double grade = -1.f;
	int point = -1;
};


enum class AnimType
{
};

void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_Gamecallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);
void CALLBACK recv_Lobbycallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

static std::shared_ptr<class Network> m_Network;
class ATinoCharacter;
class ABaseObstacle;

//lobby packet
void send_checkversion_packet(SOCKET& sock, const char* version);
void send_login_packet(SOCKET& sock,const char* id, const char* passWord); 
void send_logout_packet(SOCKET& sock);
void send_newaccount_packet(SOCKET& sock, const char* id, const char* passWord, const int department);
void send_match_packet(SOCKET& sock);
void send_matchout_packet(SOCKET& sock);
void send_control_packet(SOCKET& sock);
void send_buyitem_packet(SOCKET& sock, const int itemCode);
void send_equip_packet(SOCKET& sock, const int itemCode);
void send_unequip_packet(SOCKET& sock, const int itemCode);
void send_refresh_dep_rank_packet(SOCKET& sock);
void send_refresh_userstatus_packet(SOCKET& sock);
void send_refresh_inventory_packet(SOCKET& sock);
void send_coupon_packet(SOCKET& sock, const char* couponCode);
//game packet
void send_movetogame_packet(SOCKET& sock,const int uID, const char* id, const int& roomID);
void send_move_packet(SOCKET& sock, const bool& inair, const float& x, const float& y, const float& z, FQuat& rotate, const float& value, const FVector& speedVec);
void send_action_packet(SOCKET& sock, const char action);
void send_ping_packet(SOCKET& sock, const long long ping);
void send_goal_packet(SOCKET& sock);
void send_game_playerload_ack_packet(SOCKET& sock);
void send_game_breakdoor_packet(SOCKET& sock, const int objectID);
void send_game_breakplatform_packet(SOCKET& sock, const int objectID);


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
	void l_process_packet(unsigned char* p);

	static std::shared_ptr<class Network> GetNetwork();

	void RegisterObjectNetwork(ABaseObstacle* object,int& ObjectID);


	void LoadItemData();

	void SetObjectNetID(ABaseObstacle* object, const int netID);

public:
	void error_display(int err_no);
	bool ConnectServerGame();
	bool ConnectServerLobby();
	bool RecvPacketGame();
	bool RecvPacketLobby();

public:

	ATinoCharacter* mMyCharacter;
	FString MyCharacterName;
	FString MyCharacterPassWord;
	UPROPERTY()
	ATinoCharacter* mOtherCharacter[MAX_USER];
	UPROPERTY()
	ABaseObstacle* mObjects[MAX_OBJECT];
	int mGeneratedID;
	bool bLevelOpenTriggerEnabled = false;	//��������� ���� �ʱ�ȭ�� �ƴ�, OpenLevel�� ���� �ʱ�ȭ �϶� ������ �ɱ� ����. 
	bool bLoginFlag = false;	//�α��ν� true, Game������ ������ �ٽ� �κ�� ���ƿ� �� 
	bool bGameEndFlag = false;	//���������� ������ ��, ���Ӽ����� closesocket�� ���ִµ�, �̷� ���ؼ� ��Ʈ��ũ��������� �����ʰ� �ϱ� ����.
	//Flag�� true��� �α��� â�� ����� �ʱ� ���ؼ� �ִ� Ʈ��.
	short GameServerPort = -1;		//���Ӽ��� ���ӿ� port
	sGameResult GameResult;
	int RecentLevelNum = -1;

	GameDataManager* mGameDataManager;

private:
	bool isInit;
	int mObjectCnt = 0;
	HANDLE hMutex{};
public:
	WSADATA WSAData;
	bool bIsConnected = false;	//���Ӽ����� ������ �Ǿ�����,
	bool bIsConnectedLobby = false;		//�κ񼭹��� ������ �Ǿ�����
	int mDBUID;
	int ClientID;
	bool bGameIsStart = false;
	char hashs[MAX_NAME_SIZE];	// ���� ���� ��
	SOCKET s_socket;
	SOCKADDR_IN server_addr;
	WSA_OVER_EX recv_expover;
	int		_prev_size{};

	SOCKET l_socket;
	SOCKADDR_IN l_server_addr;
	WSA_OVER_EX l_recv_expover;
	int			l_prev_size{};
	// 112.152.55.49  127.0.0.1  , 112.153.53.142
	const char* GAMESERVER_ADDR = "127.0.0.1";
	const char* LOBBYSERVER_ADDR = "127.0.0.1";
	bool bMultiClientBan = true;
};



