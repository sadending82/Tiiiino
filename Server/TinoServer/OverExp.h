#pragma once
#include "stdafx.h"

enum eCompType
{ 
	OP_ACCEPT, OP_RECV, OP_SEND, OP_EVENT, OP_SERVER_RECV
};

class OverEXP
{
public:
	OverEXP()
	{
		mWsaBuf.len = BUF_SIZE;
		mWsaBuf.buf = reinterpret_cast<char*>(mMessageBuf);
		mCompType = eCompType::OP_RECV;
		ZeroMemory(&mOver, sizeof(mOver));
	}
	OverEXP(char* packet)
	{
		mWsaBuf.len = packet[0];
		mWsaBuf.buf = reinterpret_cast<char*>(mMessageBuf);
		ZeroMemory(&mOver, sizeof(mOver));
		mCompType = eCompType::OP_SEND;
		memcpy(mMessageBuf, packet, packet[0]);
	}
	~OverEXP() {}

public:
	WSAOVERLAPPED mOver;
	WSABUF mWsaBuf;
	unsigned char mMessageBuf[BUF_SIZE]; //-127~127
	eCompType mCompType;
	int mTargetID;
};


class ServerOverEXP
{
public:
	ServerOverEXP()
	{
		mWsaBuf.len = BUF_SIZE;
		mWsaBuf.buf = reinterpret_cast<char*>(mMessageBuf);
		mCompType = eCompType::OP_RECV;
		ZeroMemory(&mOver, sizeof(mOver));
	}
	ServerOverEXP(char* packet)
	{
		mWsaBuf.len = packet[0];
		mWsaBuf.buf = reinterpret_cast<char*>(mMessageBuf);
		ZeroMemory(&mOver, sizeof(mOver));
		mCompType = eCompType::OP_SEND;
		memcpy(mMessageBuf, packet, packet[0]);
	}
	~ServerOverEXP() {}

public:
	WSAOVERLAPPED mOver;
	WSABUF mWsaBuf;
	unsigned char mMessageBuf[BUF_SIZE]; //-127~127
	eCompType mCompType;
	int mServerTargetID;
};