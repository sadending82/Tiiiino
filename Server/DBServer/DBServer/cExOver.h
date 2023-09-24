#pragma once
#include "stdafx.h"

enum class eOpType
{
	OP_ACCEPT, OP_RECV, OP_SEND
};

class ExOver {
public:
	ExOver()
	{
		mWsaBuf.len = BUF_SIZE;
		mWsaBuf.buf = reinterpret_cast<char*>(mMessageBuf);
		mOpType = eOpType::OP_RECV;
		ZeroMemory(&mOver, sizeof(mOver));
	}
	ExOver(char* packet)
	{
		mWsaBuf.len = packet[0];
		mWsaBuf.buf = reinterpret_cast<char*>(mMessageBuf);
		ZeroMemory(&mOver, sizeof(mOver));
		mOpType = eOpType::OP_SEND;
		memcpy(mMessageBuf, packet, packet[0]);
	}

	~ExOver() {}

	void SetmOver(WSAOVERLAPPED over) { mOver = over; }
	void SetmWsaBuf(WSABUF buf) { mWsaBuf = buf; }
	void SetmMessgeBuf(char* buf) { memcpy(mMessageBuf, buf, sizeof(buf)); }
	void SetmOpType(eOpType type) { mOpType = type; }
	void SetmTargetID(int id) { mTargetID = id; }

	WSAOVERLAPPED GetmOver() { return mOver; }
	WSABUF GetmWsaBuf() { return mWsaBuf; }
	unsigned char* GetmMessageBuf() {return mMessageBuf;}
	eOpType GetmOpType() { return mOpType; }
	int GetmTargetID() { return mTargetID; }

//private:
	SOCKET mSocket;
	WSAOVERLAPPED mOver;
	WSABUF mWsaBuf;
	unsigned char mMessageBuf[BUF_SIZE];
	eOpType mOpType;
	int mTargetID;
};