#pragma once
#include <iostream>
#include "../Utility/Utility.h"

class Object
{
public:
	Object();
	virtual ~Object();

	Vector3f GetPosition() const { return mPosition; }
	void SetPosition(Vector3f pos) { mPosition = pos; }

	Vector4f GetRotate() const { return mRotate; }
	void SetRotate(Vector4f rotate) { mRotate = rotate; }

	virtual void Reset();
protected:
	Vector3f mPosition;
	Vector4f mRotate;
	
	Vector3f mScale;
	int move_time;

};

