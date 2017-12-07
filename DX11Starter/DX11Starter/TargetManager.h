#pragma once

#include<vector>
#include"Target.h"
#include"ParticleEmitter.h"

using namespace std;

class TargetManager
{
public:
	TargetManager(Mesh* mesh, Material* material, ParticleEmitter* particle, ID3D11Device* device);
	~TargetManager();

	vector<Entity*> GetTargets();
	
	void ResetTargets();
private:

	const float length = 150.0f;
	const float spacing = 5.0f;
	const int count = 30;
	const float width = 3.0f;
	const float height = 3.0f;

	vector<Entity*> targetList;
};

