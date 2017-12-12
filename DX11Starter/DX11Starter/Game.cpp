#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include <math.h>
#include <string>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	//Initialize camera
	camera = new Camera((float)width, (float)height, 0.25f * XM_PI, 0.01f, 100.0f);
	camera->SetPosition(0, 0, -53.0f);

	prevMousePos.x = width/2;
	prevMousePos.y = height/2;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	//Clean up Resource lists
	for (pair<char* const, Mesh*> &mesh : meshes)
	{
		delete mesh.second;
		mesh.second = nullptr;
	}
	meshes.clear();
	for (pair<char* const, Material*> &material : materials)
	{
		delete material.second;
		material.second = nullptr;
	}
	materials.clear();

	//Clean up sampler
	ppSampler->Release();

	//Clean up Game Objects
	while (entities.size() > 0)
	{
		delete entities[entities.size()-1];
		entities.pop_back();
	}

	delete targetManager;
	delete fireManager;

	while (lightManager->pointLights.size() > 0)
	{
		delete lightManager->pointLights[lightManager->pointLights.size() - 1];
		lightManager->pointLights.pop_back();
	}
	delete lightManager;

	skybox->depthState->Release();
	skybox->rasterState->Release();
	delete skybox;

	//Get rid of particle stuff
	fire->Release();
	additiveBlendState->Release();
	particleDepthState->Release();

	//Clean up render targets
	delete baseTarget;
	delete bloomTarget;
	delete bloomTarget2;
	delete radialTarget;

	//Clean up camera
	delete camera;

	//Clean up UI
	delete spriteBatch;
	delete spriteFont;

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	for (pair<char* const, SimpleVertexShader*> &vertexShader : vertexShaders)
	{
		delete vertexShader.second;
		vertexShader.second = nullptr;
	}
	vertexShaders.clear();
	for (pair<char* const, SimplePixelShader*> &pixelShader : pixelShaders)
	{
		delete pixelShader.second;
		pixelShader.second = nullptr;
	}
	pixelShaders.clear();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadResources();
	PrepPostProcessing();
	SetupGameWorld();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadResources()
{
	//Load Shaders
	SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");
	vertexShaders.insert(pair<char*, SimpleVertexShader*>("basicVertexShader", vertexShader));

	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");
	pixelShaders.insert(pair<char*, SimplePixelShader*>("basicPixelShader", pixelShader));

	SimpleVertexShader* shipVS = new SimpleVertexShader(device, context);
	shipVS->LoadShaderFile(L"ShipVS.cso");
	vertexShaders.insert(pair<char*, SimpleVertexShader*>("shipVS", shipVS));

	SimplePixelShader* shipPS = new SimplePixelShader(device, context);
	shipPS->LoadShaderFile(L"ShipPS.cso");
	pixelShaders.insert(pair<char*, SimplePixelShader*>("shipPS", shipPS));

	SimpleVertexShader* skyboxVS = new SimpleVertexShader(device, context);
	skyboxVS->LoadShaderFile(L"SkyboxVS.cso");
	vertexShaders.insert(pair<char*, SimpleVertexShader*>("skyboxVS", skyboxVS));

	SimplePixelShader* skyboxPS = new SimplePixelShader(device, context);
	skyboxPS->LoadShaderFile(L"SkyboxPS.cso");
	pixelShaders.insert(pair<char*, SimplePixelShader*>("skyboxPS", skyboxPS));

	SimpleVertexShader* bulletVS = new SimpleVertexShader(device, context);
	bulletVS->LoadShaderFile(L"BulletVS.cso");
	vertexShaders.insert(pair<char*, SimpleVertexShader*>("bulletVS", bulletVS));

	SimplePixelShader* bulletPS = new SimplePixelShader(device, context);
	bulletPS->LoadShaderFile(L"BulletPS.cso");
	pixelShaders.insert(pair<char*, SimplePixelShader*>("bulletPS", bulletPS));

	SimpleVertexShader* PPVS = new SimpleVertexShader(device, context);
	PPVS->LoadShaderFile(L"PPVS.cso");
	vertexShaders.insert(pair<char*, SimpleVertexShader*>("PPVS", PPVS));

	SimplePixelShader* PPPS = new SimplePixelShader(device, context);
	PPPS->LoadShaderFile(L"PPPS.cso");
	pixelShaders.insert(pair<char*, SimplePixelShader*>("PPPS", PPPS));

	SimplePixelShader* bloomPS = new SimplePixelShader(device, context);
	bloomPS->LoadShaderFile(L"BloomPS.cso");
	pixelShaders.insert(pair<char*, SimplePixelShader*>("bloomPS", bloomPS));

	SimplePixelShader* blurPS = new SimplePixelShader(device, context);
	blurPS->LoadShaderFile(L"BlurPS.cso");
	pixelShaders.insert(pair<char*, SimplePixelShader*>("blurPS", blurPS));

	SimpleVertexShader* particleVS = new SimpleVertexShader(device, context);
	particleVS->LoadShaderFile(L"ParticleVS.cso");
	vertexShaders.insert(pair<char*, SimpleVertexShader*>("particleVS", particleVS));

	SimplePixelShader* particlePS = new SimplePixelShader(device, context);
	particlePS->LoadShaderFile(L"ParticlePS.cso");
	pixelShaders.insert(pair<char*, SimplePixelShader*>("particlePS", particlePS));

	SimplePixelShader* radialPS = new SimplePixelShader(device, context);
	radialPS->LoadShaderFile(L"RadialPS.cso");
	pixelShaders.insert(pair<char*, SimplePixelShader*>("radialPS", radialPS));

	//Load textures
	ID3D11ShaderResourceView* marble = 0;
	ID3D11ShaderResourceView* playerTex = 0;
	ID3D11ShaderResourceView* enemy1 = 0;
	ID3D11ShaderResourceView* crosshairs = 0;

	//Load normal maps
	ID3D11ShaderResourceView* playerNorm = 0;
	ID3D11ShaderResourceView* enemyNorm = 0;

	fire = 0;

	CreateWICTextureFromFile(device, context, L"Assets/Textures/MarbleVeined0062.jpg", 0, &marble);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/SharpClawRacer.png", 0, &playerTex);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/Enemy.png", 0, &enemy1);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/fireParticle.jpg", 0, &fire);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/SharpClawNormal.png", 0, &playerNorm);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/EnemyNormal.png", 0, &enemyNorm);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/crosshair.png", 0, &crosshairs);

	ID3D11ShaderResourceView* sky = 0;
	CreateDDSTextureFromFile(device, L"Assets/Textures/space2.dds", 0, &sky);

	//Create sampler state
	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MipLODBias = 0;

	ID3D11SamplerState* sampler;
	device->CreateSamplerState(&samplerDesc, &sampler);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device->CreateSamplerState(&samplerDesc, &ppSampler);

	//Make materials

	materials.insert(pair<char*, Material*>("playerTex", new Material(vertexShaders.find("shipVS")->second, pixelShaders.find("shipPS")->second, playerTex, playerNorm, sampler)));
	materials.insert(pair<char*, Material*>("enemy1", new Material(vertexShaders.find("shipVS")->second, pixelShaders.find("shipPS")->second, enemy1, enemyNorm, sampler)));
	materials.insert(pair<char*, Material*>("sky", new Material(vertexShaders.find("skyboxVS")->second, pixelShaders.find("skyboxPS")->second, sky, sampler)));
	materials.insert(pair<char*, Material*>("bullet", new Material(vertexShaders.find("bulletVS")->second, pixelShaders.find("bulletPS")->second, marble, sampler)));
	materials.insert(pair<char*, Material*>("crosshairs", new Material(vertexShaders.find("basicVertexShader")->second, pixelShaders.find("basicPixelShader")->second, crosshairs, sampler)));

	//Release DirX stuff (references are added in each material)
	marble->Release();
	playerTex->Release();
	playerNorm->Release();
	enemy1->Release();
	enemyNorm->Release();
	sky->Release();
	crosshairs->Release();
	sampler->Release();

	//Load Models
	meshes.insert(pair<char*, Mesh*>("cube", new Mesh("Assets/Models/cube.obj", device)));
	meshes.insert(pair<char*, Mesh*>("cone", new Mesh("Assets/Models/cone.obj", device)));
	meshes.insert(pair<char*, Mesh*>("cylinder", new Mesh("Assets/Models/cylinder.obj", device)));
	meshes.insert(pair<char*, Mesh*>("helix", new Mesh("Assets/Models/helix.obj", device)));
	meshes.insert(pair<char*, Mesh*>("sphere", new Mesh("Assets/Models/sphere.obj", device)));
	meshes.insert(pair<char*, Mesh*>("player", new Mesh("Assets/Models/SharpClawRacer.obj", device)));
	meshes.insert(pair<char*, Mesh*>("enemy1", new Mesh("Assets/Models/Enemy.obj", device)));
	meshes.insert(pair<char*, Mesh*>("plane", new Mesh("Assets/Models/plane.obj", device)));

	//Load font for UI
	spriteBatch = new SpriteBatch(context);
	spriteFont = new SpriteFont(device, L"Assets/Arial.spritefont");
}

void Game::PrepPostProcessing()
{
	//Create 2DTexture to render to
	D3D11_TEXTURE2D_DESC ppTexDesc = {};
	ppTexDesc.Width = width;
	ppTexDesc.Height = height;
	ppTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	ppTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	ppTexDesc.ArraySize = 1;
	ppTexDesc.MipLevels = 1;
	ppTexDesc.CPUAccessFlags = 0;
	ppTexDesc.MiscFlags = 0;
	ppTexDesc.SampleDesc.Count = 1;
	ppTexDesc.SampleDesc.Quality = 0;
	ppTexDesc.Usage = D3D11_USAGE_DEFAULT;

	//Create RTV
	D3D11_RENDER_TARGET_VIEW_DESC ppRTVDesc = {};
	ppRTVDesc.Format = ppTexDesc.Format;
	ppRTVDesc.Texture2D.MipSlice = 0;
	ppRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	//Create SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC ppSRVDesc = {};
	ppSRVDesc.Format = ppTexDesc.Format;
	ppSRVDesc.Texture2D.MipLevels = 1;
	ppSRVDesc.Texture2D.MostDetailedMip = 0;
	ppSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	//Base target to render scene to
	baseTarget = new DXRenderTarget(device, ppTexDesc, ppRTVDesc, ppSRVDesc);

	//Light Bloom targets
	bloomTarget = new DXRenderTarget(device, ppTexDesc, ppRTVDesc, ppSRVDesc);
	bloomTarget2 = new DXRenderTarget(device, ppTexDesc, ppRTVDesc, ppSRVDesc);

	//Radial blur
	radialTarget = new DXRenderTarget(device, ppTexDesc, ppRTVDesc, ppSRVDesc);

}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::SetupGameWorld()
{
	//Set up lights and sky
	lightManager = new LightManager();
	skybox = new Skybox();

	//Create smoke for targets
	smoke = new ParticleEmitter(
		500,							// Max particles
		50,							// Particles per second
		0.5f,								// Particle lifetime
		1,							// Start size
		0.1f,							// End size
		XMFLOAT4(1, 0.5f, 0, 0.6f),	// Start color
		XMFLOAT4(0, 0, 0, 0.5f),		// End color
		XMFLOAT3(0, 1, 0),				// Start velocity
		XMFLOAT3(0, 0, 0),				// Start position
		XMFLOAT3(0, 2, 0),				// Start acceleration
		device,
		vertexShaders.find("particleVS")->second,
		pixelShaders.find("particlePS")->second,
		fire,
		2);

	thruster = new ParticleEmitter(
		500,							// Max particles
		50,							// Particles per second
		0.5f,								// Particle lifetime
		0.3,							// Start size
		0.1f,							// End size
		XMFLOAT4(0.4f, 0.4f, 0.4f, 0.6f),	// Start color
		XMFLOAT4(0, 0, 0, 0.5f),		// End color
		XMFLOAT3(0, 0, 1),				// Start velocity
		XMFLOAT3(0, 0, 0),				// Start position
		XMFLOAT3(0, 0, 3),				// Start acceleration
		device,
		vertexShaders.find("particleVS")->second,
		pixelShaders.find("particlePS")->second,
		fire,
		NULL);

	//Make target field
	targetManager = new TargetManager(meshes.find("enemy1")->second, materials.find("enemy1")->second, smoke, thruster, device);
	for each (Entity* e in targetManager->GetTargets())
	{
		entities.push_back(e);
		lightManager->pointLights.push_back(((Target*) e)->GetEngine());
	}

	//Make player
	player = new Player(meshes.find("player")->second, materials.find("playerTex")->second);
	lightManager->pointLights.push_back(player->GetLeftEngine());
	lightManager->pointLights.push_back(player->GetRightEngine());
	entities.push_back(player);

	//Make fire control
	fireManager = new FireManager(meshes.find("sphere")->second, materials.find("bullet")->second);
	for each (Bullet* b in fireManager->GetBullets())
	{
		entities.push_back(b);
		b->Link(player);
		lightManager->pointLights.push_back(b->GetLaser());
	}

	//Make reticule
	ID3D11BlendState* rb;
	D3D11_BLEND_DESC reticuleBlend = {};
	reticuleBlend.AlphaToCoverageEnable = false;
	reticuleBlend.IndependentBlendEnable = false;
	reticuleBlend.RenderTarget[0].BlendEnable = true;
	reticuleBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	reticuleBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	reticuleBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	reticuleBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	reticuleBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	reticuleBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	reticuleBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&reticuleBlend, &rb);
	reticule = new Reticule(meshes.find("plane")->second, materials.find("crosshairs")->second, player, targetManager, rb, fireManager->GetBullets().at(0)->GetRadius());
	reticule->SetActive(true);
	entities.push_back(reticule);

	DirectionalLight d = DirectionalLight();
	d.AmbientColor = XMFLOAT4(+0.2f, +0.2f, +0.2f, 1.0f);
	d.DiffuseColor = XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f);
	d.Direction = XMFLOAT3(+1.0f, -1.0f, +1.0f);
	lightManager->dirLight = d;

	//Create Skybox
	skybox->mesh = meshes.find("cube")->second;
	skybox->material = materials.find("sky")->second;

	D3D11_RASTERIZER_DESC rd = {};
	rd.CullMode = D3D11_CULL_FRONT;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.DepthClipEnable = true;
	device->CreateRasterizerState(&rd, &(skybox->rasterState));

	D3D11_DEPTH_STENCIL_DESC ds = {};
	ds.DepthEnable = true;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&ds, &(skybox->depthState));

	// A depth state for the particles
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &particleDepthState);


	// Blend for particles (additive)
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &additiveBlendState);

	// Set up particles
	leftThruster = new ParticleEmitter(
		500,							// Max particles
		50,							// Particles per second
		0.5f,								// Particle lifetime
		0.2f,							// Start size
		0.1f,							// End size
		XMFLOAT4(0, 0.5f, 1, 0.6f),	// Start color
		XMFLOAT4(0, 0.5f, 1, 0.2f),		// End color
		XMFLOAT3(0, 0, -1),				// Start velocity
		XMFLOAT3(0, 0, 0),				// Start position
		XMFLOAT3(0, 0, -5),				// Start acceleration
		device,
		vertexShaders.find("particleVS")->second,
		pixelShaders.find("particlePS")->second,
		fire,
		NULL);

	rightThruster = leftThruster->Clone(device);
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera->Resize((float)width, (float)height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		fireManager->Fire(deltaTime, totalTime, true);
	}
	else fireManager->Fire(deltaTime, totalTime, false);

	//player control
	float aX = 0.0f;
	float aY = 0.0f;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)
	{
		//move left
		//player->Move(-3 * deltaTime, 0, 0);
		aX = -player->accelRate * deltaTime;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
	{
		//move right
		//player->Move(3 * deltaTime, 0, 0);
		aX = player->accelRate * deltaTime;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000)
	{
		//move up (lean back)
		//player->Move(0, 3 * deltaTime, 0);
		aY = player->accelRate * deltaTime;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000)
	{
		//move down (lean forward)
		//player->Move(0, -3 * deltaTime, 0);
		aY = -player->accelRate * deltaTime;
	}
	player->Accelerate(aX, aY, 0);
	XMFLOAT3 v = player->velocity;
	player->Move(v.x, v.y, v.z);
	player->Decelerate(player->decelRate * deltaTime);

	//Update Camera
	camera->SetPosition(player->GetPosition().x / 4.0f, player->GetPosition().y / 4.0f, player->GetPosition().z - 4.0f);
	camera->Update(deltaTime, totalTime, player->GetPosition());

	//Keep thrusters close to player
	leftThruster->SetEmitterPosition(XMFLOAT3(player->GetPosition().x + 0.16f, player->GetPosition().y + 0.15f, player->GetPosition().z - 0.8f));
	leftThruster->Update(deltaTime);

	rightThruster->SetEmitterPosition(XMFLOAT3(player->GetPosition().x - 0.16f, player->GetPosition().y + 0.15f, player->GetPosition().z - 0.8f));
	rightThruster->Update(deltaTime);

	//Update Entities
	for each (Entity* e in entities)
	{
		e->Update(deltaTime, totalTime);
	}

	//Collision detection
	CheckForCollisions(fireManager->GetBullets(), targetManager->GetTargets());

	//Reset level when player passes end
	if (player->GetPosition().z > 350.0f) {
		targetManager->ResetTargets();
		player->SetPosition(player->GetPosition().x, player->GetPosition().y, -50.0f);
	}
}

void Game::CheckForCollisions(vector<Entity*> l1, vector<Entity*> l2)
{
	for (Entity* e1 : l1) {
		if (e1->IsActive()) {
			for (Entity* e2 : l2) {
				if (e2->IsActive()) {
					float distX = e1->GetPosition().x - e2->GetPosition().x;
					float distY = e1->GetPosition().y - e2->GetPosition().y;
					float distZ = e1->GetPosition().z - e2->GetPosition().z;

					float distV = pow(distX, 2) + pow(distY, 2) + pow(distZ, 2);
					if (distV < pow((e1->GetRadius() + e2->GetRadius()), 2)) {
						e1->Collides();
						e2->Collides();
						score++;
					}
				}
			}
		}
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* nothing = 0;

	//Set Target for base render
	if (postProcessing)
	{
		ID3D11RenderTargetView* baseRTV = baseTarget->GetRTV();
		context->OMSetRenderTargets(1, &baseRTV, depthStencilView);
	}
	else {
		context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	}

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f,	0);

	//Draw everything
	DrawScene(deltaTime, totalTime);

	// Turn off vertex and index buffers 
	context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	if (postProcessing == true)
	{
		DrawPostProcessing();
	}

	//Turn off srvs to prevent DX errors
	context->PSSetShaderResources(0, 16, nullSRVs);

	DrawScore();

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}

void Game::DrawScene(float deltaTime, float totalTime)
{

	//Draw all Targets
	for (size_t i = 0; i < targetManager->GetTargets().size(); i++)
	{
		((Target*)targetManager->GetTargets()[i])->Draw(context, camera, lightManager);
	}

	//Draw Bullets
	context->RSSetState(skybox->rasterState);
	for (size_t i = 0; i < fireManager->GetBullets().size(); i++)
	{
		((Bullet*)fireManager->GetBullets()[i])->Draw(context, camera, lightManager);
	}
	context->RSSetState(0);

	//Draw Player
	player->Draw(context, camera, lightManager);

	//Draw Skybox next to last
	DrawSkybox(skybox);

	//Draw reticule (transparent so has to go after skybox
	context->OMSetBlendState(reticule->GetBlend(), 0, 0xffffffff);
	reticule->Draw(context, camera, lightManager);
	ClearBlending();

	//Draw Particles! This is a three step process, so here's what you do:
	//Step 1: Set up your first blend state, like additive blending
	SetAdditiveBlending();
	

	//Step 2: Draw the emitters using that blend state
	leftThruster->Draw(context, camera);
	rightThruster->Draw(context, camera);
	for (size_t i = 0; i < targetManager->GetTargets().size(); i++)
	{
		((Target*)targetManager->GetTargets()[i])->DrawEmitter(context, camera);
	}

	//Repeat steps 1 & 2 for other blending states

	//Step 3: Reset to default states for next frame
	ClearBlending();
}

void Game::SetAdditiveBlending()
{
	float blend[4] = { 1,1,1,1 };
	context->OMSetBlendState(additiveBlendState, blend, 0xffffffff);  // Additive blending
	context->OMSetDepthStencilState(particleDepthState, 0);			// No depth WRITING
}

void Game::ClearBlending()
{
	// Reset to default states for next frame
	float blend[4] = { 1,1,1,1 };
	context->OMSetBlendState(0, blend, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);
}

void Game::DrawSkybox(Skybox* sky)
{
	Material* material = sky->material;
	Mesh* mesh = sky->mesh;
	SimpleVertexShader* vShader = material->GetVertexShader();
	SimplePixelShader* pShader = material->GetPixelShader();

	//Set Shaders
	vShader->SetShader();
	pShader->SetShader();

	//Set Shader variables
	vShader->SetMatrix4x4("view", camera->GetView());
	vShader->SetMatrix4x4("projection", camera->GetProj());
	
	pShader->SetShaderResourceView("skyboxTexture", material->GetTexture());
	pShader->SetSamplerState("basicSampler", material->GetSampler());

	//Copy data
	vShader->CopyAllBufferData();
	pShader->CopyAllBufferData();

	//Set vertex and index buffers
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mesh->GetVertexBuffer(), &stride, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	//Set rasterizer and depth states
	context->RSSetState(sky->rasterState);
	context->OMSetDepthStencilState(sky->depthState, 0);

	//DRAW!
	context->DrawIndexed(mesh->GetIndexCount(), 0, 0);

	//Reset states
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}

void Game::DrawPostProcessing()
{
	const float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	//Begin postprocessing
	SimpleVertexShader* ppVS = vertexShaders.find("PPVS")->second; //This should work for all draws to textures
	ppVS->SetShader();

	//Extract bright areas to bloom target
	ID3D11RenderTargetView* bloomRTV = bloomTarget->GetRTV();
	context->OMSetRenderTargets(1, &bloomRTV, 0);
	context->ClearRenderTargetView(bloomRTV, color);

	SimplePixelShader* bloomPS = pixelShaders.find("bloomPS")->second;
	bloomPS->SetShader();

	bloomPS->SetFloat("clipValue", clipValue);
	bloomPS->CopyAllBufferData();

	bloomPS->SetShaderResourceView("BasePixels", baseTarget->GetSRV());
	bloomPS->SetSamplerState("Sampler", ppSampler);

	context->Draw(3, 0);

	//Blur horizontally to bloomTarget2
	ID3D11RenderTargetView* bloomRTV2 = bloomTarget2->GetRTV();
	context->OMSetRenderTargets(1, &bloomRTV2, 0);
	context->ClearRenderTargetView(bloomRTV2, color);

	SimplePixelShader* blurPS = pixelShaders.find("blurPS")->second;
	blurPS->SetShader();

	blurPS->SetFloat2("passDir", horizontDir);
	blurPS->SetFloat("pixelWidth", 1.0f / width);
	blurPS->SetFloat("pixelHeight", 1.0f / height);
	blurPS->CopyAllBufferData();

	blurPS->SetShaderResourceView("BasePixels", bloomTarget->GetSRV());
	blurPS->SetSamplerState("Sampler", ppSampler);

	context->Draw(3, 0);

	//Turn off srvs to prevent DX errors
	context->PSSetShaderResources(0, 16, nullSRVs);

	//Blur vertically back to bloomTarget
	context->OMSetRenderTargets(1, &bloomRTV, 0);
	context->ClearRenderTargetView(bloomRTV, color);

	blurPS->SetFloat2("passDir", verticalDir);
	blurPS->CopyAllBufferData();

	blurPS->SetShaderResourceView("BasePixels", bloomTarget2->GetSRV());
	blurPS->SetSamplerState("Sampler", ppSampler);

	context->Draw(3, 0);

	ID3D11RenderTargetView* radialRTV = radialTarget->GetRTV();
	context->OMSetRenderTargets(1, &radialRTV, depthStencilView);
	context->ClearRenderTargetView(radialRTV, color);
	
	SimplePixelShader* radialPS = pixelShaders.find("radialPS")->second;
	radialPS->SetShader();
	radialPS->SetSamplerState("Sampler", ppSampler);
	radialPS->SetShaderResourceView("BasePixels", baseTarget->GetSRV());

	radialPS->CopyAllBufferData();

	context->Draw(3, 0);

	//End Postprocessing

	//Set OMRTV to back buffer for final draw
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Set post-processing shaders and variables
	SimplePixelShader* ppPS = pixelShaders.find("PPPS")->second;

	ppPS->SetShader();
	ppPS->SetShaderResourceView("BasePixels", baseTarget->GetSRV());
	ppPS->SetShaderResourceView("LightBloom", bloomTarget->GetSRV());
	ppPS->SetShaderResourceView("RadialBlur", radialTarget->GetSRV());
	ppPS->SetSamplerState("Sampler", ppSampler);

	//Draw to back buffer
	context->Draw(3, 0);
}

void Game::DrawScore()
{
	wstring scoreText = L"SCORE: " + to_wstring(score);
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch, scoreText.c_str(), XMFLOAT2(10.0f, (float)height - 50.0f));
	spriteBatch->End();

	// Reset any and all render states that sprite batch has changed
	float blendFactors[4] = { 1,1,1,1 };
	context->OMSetBlendState(0, blendFactors, 0xFFFFFFFF);
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}

#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
	postProcessing = !postProcessing;

}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	//Rotate camera
	//camera->Rotate((float)(x - prevMousePos.x), (float)(y - prevMousePos.y));

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion