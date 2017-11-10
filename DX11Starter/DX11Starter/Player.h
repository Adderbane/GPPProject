#pragma once
#include "Entity.h"
class Player :
	public Entity
{
public:
	Player(Mesh* mesh, Material* material);
	~Player();
	void Update(float deltaTime, float totalTime) override;
	void Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager) override;
	void Collides() override;
	void Accelerate(float dx, float dy, float dz);

	PointLight* GetLeftEngine();
	PointLight* GetRightEngine();

private:
	XMFLOAT3 velocity;
	XMFLOAT3 accel;
	float decelRate;
	float xCap;
	float yCap;

	XMFLOAT3 engineOffset;
	PointLight* leftEngine;
	PointLight* rightEngine;
};

