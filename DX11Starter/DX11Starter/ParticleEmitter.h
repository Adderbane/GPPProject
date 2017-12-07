#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "Camera.h"
#include "SimpleShader.h"
using namespace DirectX;

struct Particle
{
	XMFLOAT3 Position;
	XMFLOAT4 Color;
	XMFLOAT3 StartVel;
	float Size; 
	float Age;
};

struct ParticleVertex
{
	XMFLOAT3 Position;
	XMFLOAT2 UV;
	XMFLOAT4 Color;
	float Size;
};

class ParticleEmitter
{
public:
	ParticleEmitter(
		int maxParticles,
		int particlesPerSecond,
		float lifetime,
		float startSize,
		float endSize,
		XMFLOAT4 startColor,
		XMFLOAT4 endColor,
		XMFLOAT3 startVelocity,
		XMFLOAT3 emitterPosition,
		XMFLOAT3 emitterAcceleration,
		ID3D11Device* device,
		SimpleVertexShader* vs,
		SimplePixelShader* ps,
		ID3D11ShaderResourceView* texture,
		float emitterMaxLife);
	~ParticleEmitter();

	//Copy constructor, sort of
	ParticleEmitter* Clone(ID3D11Device* device);

	void Update(float dt);

	bool IsActive();
	void SetActive(bool active);

	void UpdateSingleParticle(float dt, int index);
	void SpawnParticle();

	void CopyParticlesToGPU(ID3D11DeviceContext* context);
	void CopyOneParticle(int index);
	void Draw(ID3D11DeviceContext* context, Camera* camera);
	void SetEmitterPosition(XMFLOAT3 pos);

private:
	// Emission properties
	int particlesPerSecond;
	float secondsPerParticle;
	float timeSinceEmit;

	int livingParticleCount;
	float lifetime;
	float emitterMaxLife;
	float emitterLife;
	bool active;

	DirectX::XMFLOAT3 emitterAcceleration;
	DirectX::XMFLOAT3 emitterPosition;
	DirectX::XMFLOAT3 startVelocity;
	DirectX::XMFLOAT4 startColor;
	DirectX::XMFLOAT4 endColor;
	float startSize;
	float endSize;

	// Particle array
	Particle* particles;
	int maxParticles;
	int firstDeadIndex;
	int firstAliveIndex;

	// Rendering
	ParticleVertex* localParticleVertices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* texture;
	SimpleVertexShader* vs;
	SimplePixelShader* ps;
};

