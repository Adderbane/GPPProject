#include "Material.h"

Material::Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* srv, ID3D11SamplerState* sampler)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->srv = srv;
	this->sampler = sampler;

	srv->AddRef();
	sampler->AddRef();
}

Material::~Material()
{
	srv->Release();
	sampler->Release();
}

//Getters
SimpleVertexShader* Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader* Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView* Material::GetSRV()
{
	return srv;
}

ID3D11SamplerState* Material::GetSampler()
{
	return sampler;
}