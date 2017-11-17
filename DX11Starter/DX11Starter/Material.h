#pragma once

#include "SimpleShader.h"
#include <memory>

class Material
{
public:
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* srv, ID3D11SamplerState* sampler);
	~Material();
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetSRV();
	ID3D11SamplerState* GetSampler();
private:
	//Shader
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	
	//Texture
	ID3D11ShaderResourceView* srv;
	ID3D11SamplerState* sampler;
};