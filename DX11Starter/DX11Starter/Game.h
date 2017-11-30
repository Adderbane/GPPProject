#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "DXRenderTarget.h"
#include "Mesh.h"
#include "Material.h"
#include "Entity.h"
#include "Player.h"
#include "Camera.h"
#include "Lights.h"
#include "TargetManager.h"
#include "FireManager.h"
#include "LightManager.h"
#include "Bullet.h"
#include "Skybox.h"
#include "ParticleEmitter.h"
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
	void CheckForCollisions(vector<Entity*> l1, vector<Entity*> l2);
	void Draw(float deltaTime, float totalTime);
	void DrawSkybox(Skybox* sky);
	void DrawScene(float deltaTime, float totalTime);
	void DrawPostProcessing();

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadResources();
	void SetupGameWorld();
	void PrepPostProcessing();

	//Game Objects
	vector<Entity*> entities;
	FireManager* fireManager;
	TargetManager* targetManager;
	LightManager* lightManager;
	Player* player;
	Skybox* skybox;

	//Resource Collections
	unordered_map<char*, Mesh*> meshes;
	unordered_map<char*, Material*> materials;
	unordered_map<char*, SimpleVertexShader*> vertexShaders;
	unordered_map<char*, SimplePixelShader*> pixelShaders;

	//Sampler
	ID3D11SamplerState* ppSampler;

	//Camera object
	Camera* camera;

	// Particle stuff
	ID3D11ShaderResourceView* fire;
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;
	ParticleEmitter* leftThruster;
	ParticleEmitter* rightThruster;

	//Postprocessing data
	bool postProcessing = true;
	DXRenderTarget* baseTarget; //Render scene to here (pre-postprocessing)
	DXRenderTarget* bloomTarget; //Render light bloom effects to here
	DXRenderTarget* bloomTarget2;
	float clipValue = 0.72f;
	float verticalDir[2] = { 0.0f, 1.0f };
	float horizontDir[2] = { 1.0f, 0.0f };

	ID3D11ShaderResourceView* nullSRVs[16] = {};

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};