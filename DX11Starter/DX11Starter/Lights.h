#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight
{
	XMFLOAT4 AmbientColor;
	XMFLOAT4 DiffuseColor;
	XMFLOAT4 SpecularColor;
	XMFLOAT3 Direction;
};

struct PointLight
{
	XMFLOAT4 AmbientColor;
	XMFLOAT4 DiffuseColor;
	XMFLOAT4 SpecularColor;
	XMFLOAT3 Position;
	float Radius;
};