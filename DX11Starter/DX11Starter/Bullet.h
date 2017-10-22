#pragma once
#include"Entity.h"
#include"Camera.h"
#include<vector>

class Bullet : public Entity
{
public:
	Bullet(Mesh* mesh, Material* material);
	~Bullet();
	void Update(float deltaTime, float totalTime) override;
	void Draw() override;
	void Collides() override;

	void Launch(float timeStamp);

	const static float range;
	const static float speed;
	const static float lifetime;
private:
	float spawnTime;
};

