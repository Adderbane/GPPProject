#include "Target.h"

Target::Target(Mesh* mesh, Material* material, ParticleEmitter* particle) : Entity(mesh, material)
{
	this->emitter = particle;
	this->emitter->SetActive(false);
	
	PointLight* e = new PointLight();
	e->AmbientColor = XMFLOAT4(0.01f, 0.00f, 0.00f, 0.0f);
	e->DiffuseColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	e->SpecularColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	e->Position = this->GetPosition();
	e->Radius = 0.1f;
	this->engine = e;
}

Target::~Target()
{
	delete emitter;
}

void Target::Update(float deltaTime, float totalTime)
{
	if (this->IsActive() != true)
	{
		emitter->SetEmitterPosition(this->GetPosition());
		emitter->Update(deltaTime);
		if (!emitter->IsActive())
		{
			engine->Radius = 0.0f;
		}
		return;
	}

	engine->Radius = 0.1f;
	engine->Position = this->GetPosition();
	engine->Position.z = this->GetPosition().z + this->GetRadius();
}

void Target::Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager)
{
	if (this->IsActive() != true)
	{
		return;
	}

	//Get array of PointLights
	PointLight lightArray[64] = {};
	for (size_t i = 0; i < lightManager->pointLights.size(); i++)
	{
		lightArray[i] = *(lightManager->pointLights[i]);
	}
	int lightCount = (int)lightManager->pointLights.size();

	SimpleVertexShader* vShader = GetMaterial()->GetVertexShader();
	SimplePixelShader* pShader = GetMaterial()->GetPixelShader();

	vShader->SetShader();
	pShader->SetShader();

	// Send data to shader variables
	vShader->SetMatrix4x4("world", GetWorld());
	vShader->SetMatrix4x4("view", camera->GetView());
	vShader->SetMatrix4x4("projection", camera->GetProj());
	vShader->SetMatrix4x4("normalWorld", GetNormalWorld());

	pShader->SetData("dirLight", &(lightManager->dirLight), sizeof(DirectionalLight));
	pShader->SetData("lightList", &lightArray, sizeof(PointLight) * 64);
	pShader->SetData("pointLightCount", &lightCount, sizeof(int));
	pShader->SetData("cameraPosition", &(camera->GetCamPosition()), sizeof(XMFLOAT3));
	pShader->SetShaderResourceView("diffuseTexture", GetMaterial()->GetTexture());
	pShader->SetShaderResourceView("normalMap", GetMaterial()->GetNormal());
	pShader->SetSamplerState("basicSampler", GetMaterial()->GetSampler());

	vShader->CopyAllBufferData();
	pShader->CopyAllBufferData();

	Entity::Draw(context, camera, lightManager);
}

void Target::DrawEmitter(ID3D11DeviceContext * context, Camera * camera)
{
	emitter->Draw(context, camera);
}

void Target::Collides()
{
	this->emitter->SetActive(true);
	this->SetActive(false);
}

PointLight* Target::GetEngine()
{
	return engine;
}
