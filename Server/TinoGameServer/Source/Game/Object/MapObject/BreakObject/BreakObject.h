#pragma once
#include "../MapObject.h"
class BreakObject : public MapObject
{
public:
	BreakObject();
	virtual ~BreakObject();

	virtual void Init();

protected:
	bool mIsBroken;
};

