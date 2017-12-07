#include "TargetManager.h"

	

TargetManager::TargetManager(Mesh* mesh, Material* material, ParticleEmitter* particle, ID3D11Device* device)
{
	for (size_t i = 0; i < this->count; i++)
	{
		Entity* t = new Target(mesh, material, particle->Clone(device));
		t->SetPosition(0.0f, -1.0f, i * this->spacing);
		t->SetActive(true);
		targetList.push_back(t);
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
		e->SetActive(true);
	}
}

vector<Entity*> TargetManager::GetTargets()
{
	return targetList;
}