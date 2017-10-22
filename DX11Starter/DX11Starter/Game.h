#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"
#include "TargetManager.h"
#include "FireManager.h"
#include "Bullet.h"
#include <vector>

using namespace std;

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadResources();
	void SetupGameWorld();

	//Game Objects
	vector<Entity*> entities;
	FireManager* fireManager;
	TargetManager* targetManager;

	//Resource Collections
	unordered_map<char*, Mesh*> meshes;
	unordered_map<char*, Material*> materials;
	unordered_map<char*, SimpleVertexShader*> vertexShaders;
	unordered_map<char*, SimplePixelShader*> pixelShaders;

	//Camera object
	Camera* camera;

	//Light objects
	DirectionalLight dirLight1;
	DirectionalLight dirLight2;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};