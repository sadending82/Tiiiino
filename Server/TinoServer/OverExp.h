#pragma once
#include "stdafx.h"

enum eCompType
{ 
	OP_ACCEPT, OP_RECV, OP_SEND, OP_EVENT
};

class OverEXP
{
public:
	OverEXP()
	{
		mWsaBuf.len = BUF_SIZE;
		mWsaBuf.buf = mMessageBuf;
		mCompType = eCompType::OP_RECV;
		ZeroMemory(&mOver, sizeof(mOver));
	}
	OverEXP(char* packet)
	{
		mWsaBuf.len = packet[0];
		mWsaBuf.buf = mMessageBuf;
		ZeroMemory(&mOver, sizeof(mOver));
		mCompType = eCompType::OP_SEND;
		memcpy(mMessageBuf, packet, packet[0]);
	}
	~OverEXP() {}

public:
	WSAOVERLAPPED mOver;
	WSABUF mWsaBuf;
	char mMessageBuf[BUF_SIZE];
	eCompType mCompType;
	int mTargetID;
};