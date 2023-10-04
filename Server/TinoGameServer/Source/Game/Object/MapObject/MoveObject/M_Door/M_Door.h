#pragma once
#include "../MoveObject.h"

class M_Door : public MoveObject
{
public:
	M_Door();
	virtual ~M_Door();

	virtual void Init(const int roomID,const int roomSyncID) override;
	virtual void Setting() override;
	virtual long long MeasureSyncTime(const long long ping) override;

	long long GetWaitMilliTime() const { return mWaitMilliTime; }

};

