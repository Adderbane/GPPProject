#include "ParticleEmitter.h"


ParticleEmitter::ParticleEmitter(
	int maxParticles,
	int particlesPerSecond,
	float lifetime,
	float startSize,
	float endSize,
	DirectX::XMFLOAT4 startColor,
	DirectX::XMFLOAT4 endColor,
	DirectX::XMFLOAT3 startVelocity,
	DirectX::XMFLOAT3 emitterPosition,
	DirectX::XMFLOAT3 emitterAcceleration,
	ID3D11Device* device,
	SimpleVertexShader* vs,
	SimplePixelShader* ps,
	ID3D11ShaderResourceView* texture,
	float emitterMaxLife
)
{
	// Save params
	this->vs = vs;
	this->ps = ps;
	this->texture = texture;

	this->maxParticles = maxParticles;
	this->lifetime = lifetime;
	this->startColor = startColor;
	this->endColor = endColor;
	this->startVelocity = startVelocity;
	this->startSize = startSize;
	this->endSize = endSize;
	this->particlesPerSecond = particlesPerSecond;
	this->secondsPerParticle = 1.0f / particlesPerSecond;

	this->emitterPosition = emitterPosition;
	this->emitterAcceleration = emitterAcceleration;
	this->emitterMaxLife = emitterMaxLife;
	this->emitterLife = 0;
	this->active = true;

	timeSinceEmit = 0;
	livingParticleCount = 0;
	firstAliveIndex = 0;
	firstDeadIndex = 0;

	// Make the particle array
	particles = new Particle[maxParticles];

	// Create local particle vertices (easier to update)
	// Do UV's here, as those will never change
	localParticleVertices = new ParticleVertex[4 * maxParticles];
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		localParticleVertices[i + 0].UV = XMFLOAT2(0, 0);
		localParticleVertices[i + 1].UV = XMFLOAT2(1, 0);
		localParticleVertices[i + 2].UV = XMFLOAT2(1, 1);
		localParticleVertices[i + 3].UV = XMFLOAT2(0, 1);
	}


	// Create buffers for drawing particles

	// DYNAMIC vertex buffer (no initial data necessary)
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;
	device->CreateBuffer(&vbDesc, 0, &vertexBuffer);

	// Index buffer data
	unsigned int* indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	// Regular index buffer
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;
	device->CreateBuffer(&ibDesc, &indexData, &indexBuffer);

	delete[] indices;
}

ParticleEmitter::~ParticleEmitter()
{
	delete[] particles;
	delete[] localParticleVertices;
	vertexBuffer->Release();
	indexBuffer->Release();
}

ParticleEmitter * ParticleEmitter::Clone(ID3D11Device * device)
{
	return new ParticleEmitter(this->maxParticles, 
		this->particlesPerSecond, 
		this->lifetime, 
		this->startSize, 
		this->endSize, 
		this->startColor, 
		this->endColor, 
		this->startVelocity,
		this->emitterPosition,
		this->emitterAcceleration,
		device,
		this->vs,
		this->ps,
		this->texture,
		this->emitterMaxLife);
}

void ParticleEmitter::Update(float dt)
{
	//if we have an active particle emitter, or the particle emitter is infinite, update it
	if (this->active) {
		this->emitterLife += dt;

		if (this->emitterMaxLife != NULL && this->emitterLife >= this->emitterMaxLife)
		{
			active = false;
			return;
		}

		// Update all particles - Check cyclic buffer first
		if (firstAliveIndex < firstDeadIndex)
		{
			// First alive is BEFORE first dead, so the "living" particles are contiguous
			// 
			// 0 -------- FIRST ALIVE ----------- FIRST DEAD -------- MAX
			// |    dead    |            alive       |         dead    |

			// First alive is before first dead, so no wrapping
			for (int i = firstAliveIndex; i < firstDeadIndex; i++)
				UpdateSingleParticle(dt, i);
		}
		else
		{
			// First alive is AFTER first dead, so the "living" particles wrap around
			// 
			// 0 -------- FIRST DEAD ----------- FIRST ALIVE -------- MAX
			// |    alive    |            dead       |         alive   |

			// Update first half (from firstAlive to max particles)
			for (int i = firstAliveIndex; i < maxParticles; i++)
				UpdateSingleParticle(dt, i);

			// Update second half (from 0 to first dead)
			for (int i = 0; i < firstDeadIndex; i++)
				UpdateSingleParticle(dt, i);
		}

		// Add to the time
		timeSinceEmit += dt;

		// Enough time to emit?
		while (timeSinceEmit > secondsPerParticle)
		{
			SpawnParticle();
			timeSinceEmit -= secondsPerParticle;
		}
	}
}

bool ParticleEmitter::IsActive()
{
	return active;
}

void ParticleEmitter::SetActive(bool active)
{
	this->active = active;
}

void ParticleEmitter::UpdateSingleParticle(float dt, int index)
{
	// Check for valid particle age before doing anything
	if (particles[index].Age >= lifetime)
		return;

	// Update and check for death
	particles[index].Age += dt;
	if (particles[index].Age >= lifetime)
	{
		// Recent death, so retire by moving alive count
		firstAliveIndex++;
		firstAliveIndex %= maxParticles;
		livingParticleCount--;
		return;
	}

	// Calculate age percentage for lerp
	float agePercent = particles[index].Age / lifetime;

	// Interpolate the color
	XMStoreFloat4(
		&particles[index].Color,
		XMVectorLerp(
			XMLoadFloat4(&startColor),
			XMLoadFloat4(&endColor),
			agePercent));

	// Lerp size
	particles[index].Size = startSize + agePercent * (endSize - startSize);


	// Adjust the position
	XMVECTOR startPos = XMLoadFloat3(&emitterPosition);
	XMVECTOR startVel = XMLoadFloat3(&particles[index].StartVel);
	XMVECTOR accel = XMLoadFloat3(&emitterAcceleration);
	float t = particles[index].Age;

	// Use constant acceleration function
	XMStoreFloat3(
		&particles[index].Position,
		accel * t * t / 2.0f + startVel * t + startPos);
}

void ParticleEmitter::SpawnParticle()
{
	// Any left to spawn?
	if (livingParticleCount == maxParticles)
		return;

	// Reset the first dead particle
	particles[firstDeadIndex].Age = 0;
	particles[firstDeadIndex].Size = startSize;
	particles[firstDeadIndex].Color = startColor;
	particles[firstDeadIndex].Position = emitterPosition;
	particles[firstDeadIndex].StartVel = startVelocity;
	particles[firstDeadIndex].StartVel.x += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVel.y += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;
	particles[firstDeadIndex].StartVel.z += ((float)rand() / RAND_MAX) * 0.4f - 0.2f;

	// Increment and wrap
	firstDeadIndex++;
	firstDeadIndex %= maxParticles;

	livingParticleCount++;
}

void ParticleEmitter::CopyParticlesToGPU(ID3D11DeviceContext * context)
{
	// Update local buffer (living particles only as a speed up)

	// Check cyclic buffer status
	if (firstAliveIndex < firstDeadIndex)
	{
		for (int i = firstAliveIndex; i < firstDeadIndex; i++)
			CopyOneParticle(i);
	}
	else
	{
		// Update first half (from firstAlive to max particles)
		for (int i = firstAliveIndex; i < maxParticles; i++)
			CopyOneParticle(i);

		// Update second half (from 0 to first dead)
		for (int i = 0; i < firstDeadIndex; i++)
			CopyOneParticle(i);
	}

	// All particles copied locally - send whole buffer to GPU
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	memcpy(mapped.pData, localParticleVertices, sizeof(ParticleVertex) * 4 * maxParticles);

	context->Unmap(vertexBuffer, 0);
}

void ParticleEmitter::CopyOneParticle(int index)
{
	int i = index * 4;

	localParticleVertices[i + 0].Position = particles[index].Position;
	localParticleVertices[i + 1].Position = particles[index].Position;
	localParticleVertices[i + 2].Position = particles[index].Position;
	localParticleVertices[i + 3].Position = particles[index].Position;

	localParticleVertices[i + 0].Size = particles[index].Size;
	localParticleVertices[i + 1].Size = particles[index].Size;
	localParticleVertices[i + 2].Size = particles[index].Size;
	localParticleVertices[i + 3].Size = particles[index].Size;

	localParticleVertices[i + 0].Color = particles[index].Color;
	localParticleVertices[i + 1].Color = particles[index].Color;
	localParticleVertices[i + 2].Color = particles[index].Color;
	localParticleVertices[i + 3].Color = particles[index].Color;
}

void ParticleEmitter::Draw(ID3D11DeviceContext * context, Camera * camera)
{
	if (!this->active)
	{
		return;
	}
	// Copy to dynamic buffer
	CopyParticlesToGPU(context);

	// Set up buffers
	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	vs->SetMatrix4x4("view", camera->GetView());
	vs->SetMatrix4x4("projection", camera->GetProj());
	vs->SetShader();
	vs->CopyAllBufferData();

	ps->SetShaderResourceView("particle", texture);
	ps->SetShader();
	ps->CopyAllBufferData();

	// Draw the correct parts of the buffer
	if (firstAliveIndex < firstDeadIndex)
	{
		context->DrawIndexed(livingParticleCount * 6, firstAliveIndex * 6, 0);
	}
	else
	{
		// Draw first half (0 -> dead)
		context->DrawIndexed(firstDeadIndex * 6, 0, 0);

		// Draw second half (alive -> max)
		context->DrawIndexed((maxParticles - firstAliveIndex) * 6, firstAliveIndex * 6, 0);
	}
}

void ParticleEmitter::SetEmitterPosition(XMFLOAT3 pos)
{
	emitterPosition = pos;
}
