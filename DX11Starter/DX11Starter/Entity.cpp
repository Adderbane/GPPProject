#include "Entity.h"

Entity::Entity(Mesh* mesh, Material* material)
{
	this->mesh = mesh;
	this->material = material;
	this->position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMStoreFloat4x4(&world, XMMatrixTranspose(XMMatrixIdentity()));
	isWorldValid = true;
	this->active = false;
	this->masterRadius = this->mesh->GetRadius();
	this->radius = this->masterRadius;
}

Entity::~Entity()
{
}

//Control active toggle
bool Entity::IsActive()
{
	return active;
}
void Entity::SetActive(bool active)
{
	this->active = active;
}

//Setters
void Entity::SetPosition(float x, float y, float z)
{
	isWorldValid = false;
	this->position = XMFLOAT3(x, y, z);
}

void Entity::SetRotation(float x, float y, float z)
{
	isWorldValid = false;
	this->rotation = XMFLOAT3(x, y, z);
}

void Entity::SetScale(float x, float y, float z)
{
	isWorldValid = false;
	this->scale = XMFLOAT3(x, y, z);
	this->radius = this->masterRadius * y;
}

//Manipulation
void Entity::Move(float dx, float dy, float dz)
{
	isWorldValid = false;
	XMVECTOR pos = XMLoadFloat3(&position);
	pos = pos + XMVectorSet(dx, dy, dz, 0.0f);
	XMStoreFloat3(&position, pos);
}

void Entity::Spin(float dx, float dy, float dz)
{
	isWorldValid = false;
	XMVECTOR rot = XMLoadFloat3(&rotation);
	rot = rot + XMVectorSet(dx, dy, dz, 0.0f);
	XMStoreFloat3(&rotation, rot);
}

void Entity::Resize(float dx, float dy, float dz)
{
	isWorldValid = false;
	XMVECTOR sca = XMLoadFloat3(&scale);
	sca = sca + XMVectorSet(dx, dy, dz, 0.0f);
	XMStoreFloat3(&scale, sca);
}

//Getters
XMFLOAT4X4 Entity::GetWorld()
{
	if (!isWorldValid)
	{
		RecalcWorld();
	}
	return world;
}

XMFLOAT4X4 Entity::GetNormalWorld()
{
	if (!isWorldValid)
	{
		RecalcWorld();
	}
	XMMATRIX worldMat = XMLoadFloat4x4(&world);
	worldMat = XMMatrixInverse(nullptr, worldMat);
	XMFLOAT4X4 normWorld;
	XMStoreFloat4x4(&normWorld, XMMatrixTranspose(worldMat));
	return normWorld;
}

XMFLOAT3 Entity::GetPosition()
{
	return position;
}

XMFLOAT3 Entity::GetRotation()
{
	return rotation;
}

XMFLOAT3 Entity::GetScale()
{
	return scale;
}

Mesh* Entity::GetMesh()
{
	return mesh;
}

Material* Entity::GetMaterial()
{
	return material;
}

float Entity::GetRadius()
{
	return radius;
}

void Entity::Update(float deltaTime, float totalTime)
{
	if (this->IsActive() != true)
	{
		return;
	}
}

void Entity::Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager)
{
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

void Entity::Collides()
{
	
}

//World calculation
void Entity::RecalcWorld()
{
	isWorldValid = true;

	//Generate matricies for each transformation type
	XMMATRIX t = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX r = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);

	//Combine and store
	XMMATRIX w = s * r * t;
	XMStoreFloat4x4(&world, XMMatrixTranspose(w));
}