#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"

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

	XMStoreFloat4(&dirLight1.AmbientColor, XMVectorSet(+0.1f, +0.1f, +0.1f, 1.0f));
	XMStoreFloat4(&dirLight1.DiffuseColor, XMVectorSet(+1.0f, +1.0f, +1.0f, 1.0f));
	XMStoreFloat3(&dirLight1.Direction,    XMVectorSet(+1.0f, -1.0f, +1.0f, 0.0f));

	XMStoreFloat4(&dirLight2.AmbientColor, XMVectorSet(+0.0f, +0.0f, +0.0f, 1.0f));
	XMStoreFloat4(&dirLight2.DiffuseColor, XMVectorSet(+0.0f, +0.0f, +0.0f, 1.0f));
	XMStoreFloat3(&dirLight2.Direction,    XMVectorSet(-1.0f, -1.0f, +1.0f, 0.0f));

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
	SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");
	vertexShaders.insert(pair<char*, SimpleVertexShader*>("basicVertexShader", vertexShader));

	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");
	pixelShaders.insert(pair<char*, SimplePixelShader*>("basicPixelShader", pixelShader));

	//Load textures
	ID3D11ShaderResourceView* wood = 0;
	ID3D11ShaderResourceView* metal = 0;
	ID3D11ShaderResourceView* marble = 0;
	CreateWICTextureFromFile(device, context, L"Assets/Textures/WoodFine0074.jpg", 0, &wood);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/BronzeCopper0011.jpg", 0, &metal);
	CreateWICTextureFromFile(device, context, L"Assets/Textures/MarbleVeined0062.jpg", 0, &marble);

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

	//Release DirX stuff (references are added in each material)
	wood->Release();
	metal->Release();
	marble->Release();
	sampler->Release();

	//Load Models
	meshes.insert(pair<char*, Mesh*>("cube", new Mesh("Assets/Models/cube.obj", device)));
	meshes.insert(pair<char*, Mesh*>("cone", new Mesh("Assets/Models/cone.obj", device)));
	meshes.insert(pair<char*, Mesh*>("cylinder", new Mesh("Assets/Models/cylinder.obj", device)));
	meshes.insert(pair<char*, Mesh*>("helix", new Mesh("Assets/Models/helix.obj", device)));
	meshes.insert(pair<char*, Mesh*>("sphere", new Mesh("Assets/Models/sphere.obj", device)));
}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::SetupGameWorld()
{
	//Make target field
	targetManager = new TargetManager(meshes.find("cube")->second, materials.find("metal")->second);
	for each (Entity* e in targetManager->GetTargets())
	{
		entities.push_back(e);
	}
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

	//Update Camera
	camera->Update(deltaTime, totalTime);
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

	//Draw all entities
	for (size_t i = 0; i < entities.size(); i++)
	{
		Entity* entity = entities[i];

		//Skip drawing if entity is not active
		if (entity->IsActive() != true)
		{
			continue;
		}

		Mesh* mesh = entity->GetMesh();
		SimpleVertexShader* vShader = entity->GetMaterial()->GetVertexShader();
		SimplePixelShader* pShader = entity->GetMaterial()->GetPixelShader();

		// Set the vertex and pixel shaders to use for the next Draw() command
		//  - These don't technically need to be set every frame...YET
		//  - Once you start applying different shaders to different objects,
		//    you'll need to swap the current shaders before each draw
		vShader->SetShader();
		pShader->SetShader();

		// Send data to shader variables
		//  - Do this ONCE PER OBJECT you're drawing
		//  - This is actually a complex process of copying data to a local buffer
		//    and then copying that entire buffer to the GPU.  
		//  - The "SimpleShader" class handles all of that for you.
		vShader->SetMatrix4x4("world", entity->GetWorld());
		vShader->SetMatrix4x4("view", camera->GetView());
		vShader->SetMatrix4x4("projection", camera->GetProj());
		vShader->SetMatrix4x4("normalWorld", entity->GetNormalWorld());

		pShader->SetData("dirLight1", &dirLight1, sizeof(DirectionalLight));
		pShader->SetData("dirLight2", &dirLight2, sizeof(DirectionalLight));
		pShader->SetData("cameraPosition", &(camera->GetCamPosition()), sizeof(XMFLOAT3));
		pShader->SetShaderResourceView("diffuseTexture", entity->GetMaterial()->GetSRV());
		pShader->SetSamplerState("basicSampler", entity->GetMaterial()->GetSampler());

		// Once you've set all of the data you care to change for
		// the next draw call, you need to actually send it to the GPU
		//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
		vShader->CopyAllBufferData();
		pShader->CopyAllBufferData();

		// Set buffers in the input assembler
		//  - Do this ONCE PER OBJECT you're drawing, since each object might
		//    have different geometry.
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh->GetVertexBuffer(), &stride, &offset);
		context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		// Finally do the actual drawing
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		context->DrawIndexed(
			mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
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
	camera->Rotate((float)(x - prevMousePos.x), (float)(y - prevMousePos.y));

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