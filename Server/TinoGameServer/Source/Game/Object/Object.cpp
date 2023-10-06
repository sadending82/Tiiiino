#include "Object.h"

Object::Object()
{
}

Object::~Object()
{
}

void Object::Reset()
{
	mPosition = {0, 0, 0};
	mRotate = { 0,0,0,0 };

	mScale = { 0,0,0 };
	move_time = 0;
}
