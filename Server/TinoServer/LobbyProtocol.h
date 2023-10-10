#pragma once

enum eEVENT_TYPE
{
	EV_MATCH_UP, EV_CONTROL
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

struct EV_ControlPacket : public TimerPacket
{

};
#pragma pack (pop)