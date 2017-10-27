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

void Entity::Draw()
{
	if (this->IsActive() != true)
	{
		return;
	}
}

void Entity::Collides()
{
	this->SetActive(false);
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