#pragma once

enum eEVENT_TYPE
{
	EV_MATCH_UP, EV_COUNT_DOWN
};

#pragma pack (push, 1)
struct TimerPacket
{
	unsigned char size;
	char	type;
};

struct EV_UpdateMatchPacket : public TimerPacket
{

};

struct EV_ReadyToGamePacket : public TimerPacket
{

};

struct EV_CountDownPacket : public TimerPacket
{
	int listnum;
};
#pragma pack (pop)