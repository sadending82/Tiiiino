// Fill out your copyright notice in the Description page of Project Settings.
// 언리얼 클래스에서 헤더파일은 generated.h아래에 추가하면 에러가 발생하므로 위에 추가해야합니다.
#pragma once

#include <iostream>
#include <memory>
#include <WS2tcpip.h>
#include "../../../../protocol.h"
#include "CoreMinimal.h"




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
void send_login_packet(SOCKET& sock,const char* id, const char* passWord); 
void send_newaccount_packet(SOCKET& sock, const char* id, const char* passWord, const int department);
void send_match_packet(SOCKET& sock);
void send_matchout_packet(SOCKET& sock);
void send_control_packet(SOCKET& sock);
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

public:
	void SetObjectNetID(ABaseObstacle* object, const int netID);

	ATinoCharacter* mMyCharacter;
	FString MyCharacterName;
	FString MyCharacterPassWord;
	UPROPERTY()
	ATinoCharacter* mOtherCharacter[MAX_USER];
	UPROPERTY()
	ABaseObstacle* mObjects[MAX_OBJECT];
	int mGeneratedID;
	bool bLevelOpenTriggerEnabled = false;	//게임종료로 인한 초기화가 아닌, OpenLevel로 인한 초기화 일때 제한을 걸기 위함. 
	bool bLoginFlag = false;	//로그인시 true, Game한판이 끝나고 다시 로비로 돌아올 때 
	//Flag가 true라면 로그인 창을 띄우지 않기 위해서 있는 트릭.
	short GameServerPort = -1;		//게임서버 접속용 port
	sGameResult GameResult;
private:
	bool isInit;
	int mObjectCnt = 0;

public:
	WSADATA WSAData;
	void error_display(int err_no);
	bool bIsConnected = false;	//게임서버랑 연결이 되었는지,
	bool bIsConnectedLobby = false;		//로비서버랑 연결이 되었는지
	int mDBUID;
	int ClientID;
	bool bGameIsStart = false;
	char hashs[MAX_NAME_SIZE];	// 서버 검증 값
	bool ConnectServerGame();
	bool ConnectServerLobby();
	SOCKET s_socket;
	SOCKADDR_IN server_addr;
	WSA_OVER_EX recv_expover;
	int		_prev_size{};

	SOCKET l_socket;
	SOCKADDR_IN l_server_addr;
	WSA_OVER_EX l_recv_expover;
	int			l_prev_size{};
	bool RecvPacketGame() ;
	bool RecvPacketLobby();
	// 112.152.55.49  127.0.0.1  , 112.153.53.142
	const char* GAMESERVER_ADDR = "127.0.0.1";
	const char* LOBBYSERVER_ADDR = "127.0.0.1";
};



