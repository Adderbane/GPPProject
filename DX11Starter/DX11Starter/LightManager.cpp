#include "LightManager.h"



LightManager::LightManager()
{
}


LightManager::~LightManager()
{
	while (pointLights.size() > 0)
	{
		delete pointLights[pointLights.size() - 1];
		pointLights.pop_back();
	}
}
