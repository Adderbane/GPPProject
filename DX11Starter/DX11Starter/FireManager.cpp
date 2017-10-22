#include "FireManager.h"

FireManager::FireManager(Mesh* mesh, Material* material)
{
	//Generates all bullets needed, so we can use bulletList as circular buffer
	for (size_t i = 0; i < maxShots; i++)
	{
		Bullet* b = new Bullet(mesh, material);
		bulletList.push_back(b);
	}
}

FireManager::~FireManager()
{
}

//Fires the first inactive bullet if ready
void FireManager::Fire(float deltaTime, float totalTime, bool fire)
{
	//Run timer if reloading (whether or not player is firing)
	if (!readyFire)
	{
		resetTimer += deltaTime;
		//Once reloaded, reset timer and set ready
		if (resetTimer >= fireDelay)
		{
			readyFire = true;
			resetTimer = 0.0f;
		}
	}
	else
	{
		//Fire a bullet if the player is firing
		if (fire)
		{
			readyFire = false;
			for (size_t i = 0; i < bulletList.size(); i++)
			{
				if (!bulletList.at(i)->IsActive())
				{
					bulletList.at(i)->Launch(totalTime);
					break;
				}
			}
		}
	}
}

vector<Bullet*> FireManager::GetBullets()
{
	return bulletList;
}