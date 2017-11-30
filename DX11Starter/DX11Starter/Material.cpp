#include "Material.h"

Material::Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* texture, ID3D11SamplerState* sampler)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->texture = texture;
	this->sampler = sampler;

	texture->AddRef();
	sampler->AddRef();
}

Material::Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalMap, ID3D11SamplerState* sampler) : Material(vertexShader, pixelShader, texture, sampler)
{
	this->normalMap = normalMap;

	normalMap->AddRef();
}

Material::~Material()
{
	if (normalMap != 0) normalMap->Release();

	texture->Release();
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

ID3D11ShaderResourceView* Material::GetTexture()
{
	return texture;
}

ID3D11ShaderResourceView* Material::GetNormal()
{
	return normalMap;
}

ID3D11SamplerState* Material::GetSampler()
{
	return sampler;
}