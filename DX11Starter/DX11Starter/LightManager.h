#pragma once
#include<vector>
#include "Lights.h"

using namespace std;

class LightManager
{
public:
	LightManager();
	~LightManager();
	DirectionalLight dirLight;
	vector<PointLight> pointLights;
};

