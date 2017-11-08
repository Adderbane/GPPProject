#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include <math.h>

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
	//Initialize fields
	camera = new Camera((float)width, (float)height, 0.25f * XM_PI, 0.01f, 100.0f);
	camera->SetPosition(0, 0, -3);

	lightManager = new LightManager();
	DirectionalLight d = DirectionalLight();
	d.AmbientColor = XMFLOAT4(+0.1f, +0.1f, +0.1f, 1.0f);
	d.DiffuseColor = XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f);
	d.Direction = XMFLOAT3(+1.0f, -1.0f, +1.0f);
	lightManager->dirLight = d;

	skybox = new Skybox();

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

	//Clean up camera
	delete camera;

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


	//Load textures
	ID3D11ShaderResourceView* wood = 0;
	ID3D11ShaderResourceView* metal = 0;
	ID3D11ShaderResourceView* marble = 0;
	ID3D11ShaderResourceView* playerTex = 0;
	ID3D11ShaderResourceView* enemy1 = 0;
	CreateWICTextureFromFile(device, context, L"Assets/Textures/WoodFine0074.jpg", 0, &wood);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/BronzeCopper0011.jpg", 0, &metal);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/MarbleVeined0062.jpg", 0, &marble);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/SharpClawRacer.png", 0, &playerTex);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/Enemy.png", 0, &enemy1);

	ID3D11ShaderResourceView* sky = 0;
	CreateDDSTextureFromFile(device, L"Assets/Textures/SunnyCubeMap.dds", 0, &sky);

	//Create sampler state
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MipLODBias = 0;

	device->CreateSamplerState(&samplerDesc, &sampler);

	//Make materials
	materials.insert(pair<char*, Material*>("wood", new Material(vertexShaders.find("basicVertexShader")->second, pixelShaders.find("basicPixelShader")->second, wood, sampler)));
	materials.insert(pair<char*, Material*>("metal", new Material(vertexShaders.find("basicVertexShader")->second, pixelShaders.find("basicPixelShader")->second, metal, sampler)));
	materials.insert(pair<char*, Material*>("marble", new Material(vertexShaders.find("basicVertexShader")->second, pixelShaders.find("basicPixelShader")->second, marble, sampler)));
	materials.insert(pair<char*, Material*>("playerTex", new Material(vertexShaders.find("basicVertexShader")->second, pixelShaders.find("basicPixelShader")->second, playerTex, sampler)));
	materials.insert(pair<char*, Material*>("enemy1", new Material(vertexShaders.find("basicVertexShader")->second, pixelShaders.find("basicPixelShader")->second, enemy1, sampler)));
	materials.insert(pair<char*, Material*>("sky", new Material(vertexShaders.find("skyboxVS")->second, pixelShaders.find("skyboxPS")->second, sky, sampler)));
	materials.insert(pair<char*, Material*>("bullet", new Material(vertexShaders.find("bulletVS")->second, pixelShaders.find("bulletPS")->second, marble, sampler)));

	//Release DirX stuff (references are added in each material)
	wood->Release();
	metal->Release();
	marble->Release();
	playerTex->Release();
	enemy1->Release();
	sky->Release();
	sampler->Release();

	//Load Models
	meshes.insert(pair<char*, Mesh*>("cube", new Mesh("Assets/Models/cube.obj", device)));
	meshes.insert(pair<char*, Mesh*>("cone", new Mesh("Assets/Models/cone.obj", device)));
	meshes.insert(pair<char*, Mesh*>("cylinder", new Mesh("Assets/Models/cylinder.obj", device)));
	meshes.insert(pair<char*, Mesh*>("helix", new Mesh("Assets/Models/helix.obj", device)));
	meshes.insert(pair<char*, Mesh*>("sphere", new Mesh("Assets/Models/sphere.obj", device)));
	meshes.insert(pair<char*, Mesh*>("player", new Mesh("Assets/Models/SharpClawRacer.obj", device)));
	meshes.insert(pair<char*, Mesh*>("enemy1", new Mesh("Assets/Models/Enemy.obj", device)));
}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::SetupGameWorld()
{
	//Make target field
	targetManager = new TargetManager(meshes.find("enemy1")->second, materials.find("enemy1")->second);
	for each (Entity* e in targetManager->GetTargets())
	{
		entities.push_back(e);
	}

	//Make player
	player = new Player(meshes.find("player")->second, materials.find("playerTex")->second);
	entities.push_back(player);

	//Make fire control
	fireManager = new FireManager(meshes.find("sphere")->second, materials.find("bullet")->second);
	for each (Bullet* b in fireManager->GetBullets())
	{
		entities.push_back(b);
		b->Link(player);
		lightManager->pointLights.push_back(b->GetLaser());
	}

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

	////Point Light Test
	//PointLight p = PointLight();
	//p.AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 0.01f);
	//p.DiffuseColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	//p.Position = XMFLOAT3(2.0f, 0.0f, 0.0f);
	//p.Radius = 0.25f;
	//lightManager->pointLights.push_back(p);
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
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)
	{
		//move left
		player->Move(-3 * deltaTime, 0, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
	{
		//move right
		player->Move(3 * deltaTime, 0, 0);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000)
	{
		//move up (lean back)
		player->Move(0, 3 * deltaTime, 0);
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000)
	{
		//move down (lean forward)
		player->Move(0, -3 * deltaTime, 0);
	}

	//Update Camera
	camera->SetPosition(player->GetPosition().x / 4, player->GetPosition().y / 4, player->GetPosition().z - 4);
	camera->Update(deltaTime, totalTime, player->GetPosition());

	//Update Entities
	for each (Entity* e in entities)
	{
		e->Update(deltaTime, totalTime);
	}

	//Collision detection
	CheckForCollisions(fireManager->GetBullets(), targetManager->GetTargets());
  
}

void Game::CheckForCollisions(vector<Bullet*> l1, vector<Entity*> l2)
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
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//Draw all Targets
	for (size_t i = 0; i < targetManager->GetTargets().size(); i++)
	{
		targetManager->GetTargets()[i]->Draw(context, camera, lightManager);
	}

	//Draw Bullets
	context->RSSetState(skybox->rasterState);
	for (size_t i = 0; i < fireManager->GetBullets().size(); i++)
	{
		fireManager->GetBullets()[i]->Draw(context, camera, lightManager);
	}
	context->RSSetState(0);

	//Draw Player
	player->Draw(context, camera, lightManager);

	//Draw Skybox last
	DrawSkybox(skybox);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
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
	
	pShader->SetShaderResourceView("skyboxTexture", material->GetSRV());
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