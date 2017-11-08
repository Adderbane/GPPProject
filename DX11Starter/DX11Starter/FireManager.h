#pragma once

#include"Bullet.h"
#include<vector>

using namespace std;

class FireManager
{
public:
	FireManager(Mesh* mesh, Material* material);
	~FireManager();
	void Fire(float deltaTime, float totalTime, bool fire);
	vector<Bullet*> GetBullets();
private:
	const float fireDelay = 0.5f;
	const int maxShots = (int) ceil(Bullet::lifetime / fireDelay) + 1;
	vector<Bullet*> bulletList;

	//Fire control variables
	float resetTimer = 0.0f;
	bool readyFire = true;
};

