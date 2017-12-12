#include "Target.h"

Target::Target(Mesh* mesh, Material* material, ParticleEmitter* explosion, ParticleEmitter* thruster) : Entity(mesh, material)
{
	this->explosion = explosion;
	this->explosion->SetActive(false);
	this->thruster = thruster;
}

Target::~Target()
{
	delete explosion;
	delete thruster;
}

void Target::Update(float deltaTime, float totalTime)
{
	if (this->IsActive() != true)
	{
		explosion->SetEmitterPosition(this->GetPosition());
		explosion->Update(deltaTime);
		return;
	}

	thruster->SetEmitterPosition(XMFLOAT3(this->GetPosition().x, this->GetPosition().y + 0.15f, this->GetPosition().z + 0.3f));
	thruster->Update(deltaTime);
}

void Target::Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager)
{
	if (this->IsActive() != true)
	{
		return;
	}

	//Get array of PointLights
	PointLight lightArray[32] = {};
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
	pShader->SetData("lightList", &lightArray, sizeof(PointLight) * 32);
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
	explosion->Draw(context, camera);
	thruster->Draw(context, camera);
}

void Target::Collides()
{
	this->explosion->SetActive(true);
	this->thruster->SetActive(false);
	this->SetActive(false);
}
