#pragma once

#include <d3d11.h>
#include "Material.h"
#include "Mesh.h"

struct Skybox
{
	Material* material;
	Mesh* mesh;
	ID3D11RasterizerState* rasterState;
	ID3D11DepthStencilState* depthState;
};