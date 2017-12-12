#pragma once
#include "Entity.h"
#include "ParticleEmitter.h"

class Target : public Entity
{
public:
	Target(Mesh* mesh, Material* material, ParticleEmitter* explosion, ParticleEmitter* thruster);
	~Target();

	void Update(float deltaTime, float totalTime) override;
	void Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager) override;
	void DrawEmitter(ID3D11DeviceContext* context, Camera* camera);
	void Collides() override;
	PointLight* GetEngine();

private:
	ParticleEmitter* explosion;
	ParticleEmitter* thruster;
	PointLight* engine;
};


