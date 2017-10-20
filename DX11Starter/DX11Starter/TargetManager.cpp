#include "TargetManager.h"

	

TargetManager::TargetManager(Mesh* mesh, Material* material)
{
	for (size_t i = 0; i < this->count; i++)
	{
		Entity* e = new Entity(mesh, material);
		e->SetPosition(0.0f, -1.0f, i * this->spacing);
		e->SetActive(true);
		targetList.push_back(e);
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