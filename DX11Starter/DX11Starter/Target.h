#pragma once
#include "Entity.h"

class Target : public Entity
{
public:
	Target(Mesh* mesh, Material* material);
	~Target();

	void Update(float deltaTime, float totalTime) override;
	void Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager) override;
	void Collides() override;
};

