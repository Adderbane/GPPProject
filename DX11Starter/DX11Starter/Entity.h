#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"

using namespace DirectX;

class Entity
{
public:
	Entity(Mesh* mesh, Material* material);
	~Entity();

	//Control active toggle
	bool IsActive();
	void SetActive(bool active);

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void Move(float dx, float dy, float dz);
	void Spin(float dx, float dy, float dz);
	void Resize(float dx, float dy, float dz);

	XMFLOAT4X4 GetWorld();
	XMFLOAT4X4 GetNormalWorld();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();
	Mesh* GetMesh();
	Material* GetMaterial();

private:

	bool active;

	//Resources
	Mesh* mesh;
	Material* material;

	//Drawing data
	XMFLOAT4X4 world;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
	bool isWorldValid;

	void RecalcWorld();
};

