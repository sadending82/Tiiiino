#pragma once
#include "../MapObject.h"
class BreakObject : public MapObject
{
	BreakObject();
	virtual ~BreakObject();

	virtual void Init();

protected:
	bool mIsBroken;
};

