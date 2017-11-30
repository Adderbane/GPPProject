#pragma once
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"
#include<vector>

class Bullet : public Entity
{
public:
	Bullet(Mesh* mesh, Material* material);
	~Bullet();
	void Update(float deltaTime, float totalTime) override;
	void Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager) override;
	void Collides() override;

	void Launch(float timeStamp);
	void Link(Entity* player);

	PointLight* GetLaser();

	const static float range;
	const static float speed;
	const static float lifetime;
	const static float laserRadius;
private:
	float spawnTime;
	PointLight* laser = 0;
	Entity* player = 0;
};

