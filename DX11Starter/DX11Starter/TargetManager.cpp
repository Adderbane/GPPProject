#include "TargetManager.h"

	

TargetManager::TargetManager(Mesh* mesh, Material* material, ParticleEmitter* explosion, ParticleEmitter* thruster, ID3D11Device* device)
{

	if (spawnFixed) {
		for (size_t i = 0; i < this->count; i++)
		{
			Entity* t = new Target(mesh, material, explosion->Clone(device), thruster->Clone(device));
			t->SetPosition(0.0f, -1.0f, i * this->spacing);
			t->SetActive(true);
			targetList.push_back(t);
		}
	}
	else {
		//spawn randomly
		for (size_t i = 0; i < this->count; i++) {
			Entity* t = new Target(mesh, material, explosion->Clone(device), thruster->Clone(device));
			float spawnX = rand() % (int)(2 * xCap) - xCap;
			float spawnY = rand() % (int)(2 * yCap) - yCap;
			t->SetPosition(spawnX, spawnY, i * this->spacing);
			t->SetActive(true);
			targetList.push_back(t);
		}
	}
}

TargetManager::~TargetManager()
{
}

//Use this method to repopulate level 
void TargetManager::ResetTargets()
{
	for each (Entity* e in targetList)
	{
		float spawnX = rand() % (int)(2 * xCap) - xCap;
		float spawnY = rand() % (int)(2 * yCap) - yCap;
		e->SetPosition(spawnX, spawnY, e->GetPosition().z);
		e->SetActive(true);
	}
}

vector<Entity*> TargetManager::GetTargets()
{
	return targetList;
}