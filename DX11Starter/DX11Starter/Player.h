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
	void Decelerate(float d);
	float accelRate;
	float decelRate;
	XMFLOAT3 velocity;

	PointLight* GetLeftEngine();
	PointLight* GetRightEngine();

private:
	
	float maxVelocity;
	XMFLOAT3 accel;
	float xCap;
	float yCap;

	XMFLOAT3 engineOffset;
	PointLight* leftEngine;
	PointLight* rightEngine;
};

