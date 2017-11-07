#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct Light
{
	XMFLOAT4 AmbientColor;
	XMFLOAT4 DiffuseColor;
};

struct DirectionalLight : Light
{
	XMFLOAT3 Direction;
};

struct PointLight : Light
{
	XMFLOAT3 Position;
	float Radius;
};