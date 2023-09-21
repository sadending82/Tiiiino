#pragma once

enum class eEVENT_TYPE
{
	EV_MATCH_UP, EV_MATCH_IN, EV_MATCH_OUT
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
#pragma pack (pop)