#pragma once
#include <iostream>

struct Vector3f {
	float x, y, z;
	Vector3f() = default;
	Vector3f(float x, float y, float z) : x(x), y(y), z(z) {};
};
struct Vector4f {
	float x, y, z, w;
	Vector4f() = default;
	Vector4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
};
class Object
{
public:
	Object();
	virtual ~Object();

	Vector3f GetPosition() const { return mPosition; }
	void SetPosition(Vector3f pos) { mPosition = pos; }

	Vector4f GetRotate() const { return mRotate; }
	void SetRotate(Vector4f rotate) { mRotate = rotate; }

protected:
	Vector3f mPosition;
	Vector4f mRotate;
	
	Vector3f mScale;

};

