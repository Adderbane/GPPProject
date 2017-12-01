#pragma once

#include "SimpleShader.h"
#include <memory>

class Material
{
public:
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* texture, ID3D11SamplerState* sampler);
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalMap, ID3D11SamplerState* sampler);
	~Material();
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* GetNormal();
	ID3D11SamplerState* GetSampler();
private:
	//Shader
	SimpleVertexShader* vertexShader = 0;
	SimplePixelShader* pixelShader = 0;
	
	//Texture
	ID3D11ShaderResourceView* texture = 0;
	ID3D11ShaderResourceView* normalMap = 0;
	ID3D11SamplerState* sampler = 0;
};