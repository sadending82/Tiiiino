#include "M_Door.h"

M_Door::M_Door()
	:mSyncTime()
{
}

M_Door::~M_Door()
{
}

long long M_Door::MeasureSyncTime(const long long ping)
{
	return mSyncTime.count() - ping;
}

