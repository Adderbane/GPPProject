#pragma once
#include"Entity.h"

class Bullet : Entity
{
public:
	Bullet();
	~Bullet();
	void Collides();
private:
	const float range = 60.0f;
	const float speed = 15.0f;
	const float lifetime = range / speed;
};

