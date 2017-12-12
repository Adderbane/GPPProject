#pragma once

#include<vector>
#include"Target.h"
#include"ParticleEmitter.h"

using namespace std;

class TargetManager
{
public:
	TargetManager(Mesh* mesh, Material* material, ParticleEmitter* explosion, ParticleEmitter* thruster, ID3D11Device* device);
	~TargetManager();

	vector<Entity*> GetTargets();
	
	void ResetTargets();
private:

	const float length = 150.0f;
	const float spacing = 5.0f;
	const int count = 30;
	const float width = 3.0f;
	const float height = 3.0f;
	const bool spawnFixed = false;
	const float xCap = 4.0f;
	const float yCap = 2.0f;

	vector<Entity*> targetList;
};

