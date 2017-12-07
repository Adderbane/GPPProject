#pragma once
#include "Entity.h"
#include "Player.h"
#include "TargetManager.h"
#include "Bullet.h"

class Reticule : public Entity
{
public:
	Reticule(Mesh* mesh, Material* material, Player* player, TargetManager* targetManager, ID3D11BlendState* blend, float bulletRad);
	~Reticule();

	void Update(float deltaTime, float totalTime) override;
	void Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager) override;

	ID3D11BlendState* GetBlend();

private:
	Player* playerRef;
	TargetManager* targetRef;
	ID3D11BlendState* blend;
	float bulletRadSq;
};

