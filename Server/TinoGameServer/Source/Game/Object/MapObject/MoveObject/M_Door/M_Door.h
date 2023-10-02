#pragma once
#include "../MoveObject.h"

class M_Door : public MoveObject
{
public:
	M_Door();
	virtual ~M_Door();

	long long MeasureSyncTime(const long long ping);
protected:
	std::chrono::milliseconds mSyncTime;
};

