#include "Player.h"

using namespace std;

Player::Player(Mesh* mesh, Material* material) : Entity(mesh, material)
{
	this->SetActive(true);
	this->SetRotation(-1.0f * XM_PIDIV2, XM_PI, 0.0f);
	accelRate = 0.2;
	decelRate = 0.09;
	velocity = XMFLOAT3(0, 0, 0);
	maxVelocity = 0.8;
	xCap = 4.0f;
	yCap = 2.0f;

	//Engine Lights
	engineOffset = XMFLOAT3(0.159718f, 0.139871f, -0.72747f); //Got from model
	leftEngine = new PointLight();
	leftEngine->AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 0.0f);
	leftEngine->DiffuseColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	leftEngine->SpecularColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	leftEngine->Position = this->GetPosition();
	leftEngine->Radius = 0.005f;
	rightEngine = new PointLight();
	rightEngine->AmbientColor = XMFLOAT4(0.01f, 0.01f, 0.01f, 0.0f);
	rightEngine->DiffuseColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	rightEngine->SpecularColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	rightEngine->Position = this->GetPosition();
	rightEngine->Radius = 0.005f;
}


Player::~Player()
{
}

void Player::Update(float deltaTime, float totalTime)
{
	XMFLOAT3 pos = GetPosition();
	Entity::Update(deltaTime, totalTime);
	this->Move(0, 0, 2.0f * deltaTime);
	if (pos.x > xCap) {
		pos.x = xCap;
	}
	if (pos.y > yCap) {
		pos.y = yCap;
	}
	if (pos.x < -xCap) {
		pos.x = -xCap;
	}
	if (pos.y < -yCap) {
		pos.y = -yCap;
	}
	this->SetPosition(pos.x, pos.y, GetPosition().z);
	//Update Engine lights
	leftEngine->Position = XMFLOAT3(pos.x - engineOffset.x, pos.y + engineOffset.y, pos.z + engineOffset.z);
	rightEngine->Position = XMFLOAT3(pos.x + engineOffset.x, pos.y + engineOffset.y, pos.z + engineOffset.z);
}

void Player::Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager)
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

	vShader->SetMatrix4x4("world", GetWorld());
	vShader->SetMatrix4x4("view", camera->GetView());
	vShader->SetMatrix4x4("projection", camera->GetProj());
	vShader->SetMatrix4x4("normalWorld", GetNormalWorld());

	pShader->SetData("dirLight", &(lightManager->dirLight), sizeof(DirectionalLight));
	pShader->SetData("lightList", &lightArray, sizeof(PointLight) * 32);
	pShader->SetData("pointLightCount", &lightCount, sizeof(int));
	pShader->SetData("cameraPosition", &(camera->GetCamPosition()), sizeof(XMFLOAT3));
	pShader->SetShaderResourceView("diffuseTexture", GetMaterial()->GetSRV());
	pShader->SetSamplerState("basicSampler", GetMaterial()->GetSampler());

	vShader->CopyAllBufferData();
	pShader->CopyAllBufferData();

	Entity::Draw(context, camera, lightManager);
}

void Player::Collides()
{
}

void Player::Accelerate(float dx, float dy, float dz)
{
	velocity.x += dx;
	if (velocity.x > maxVelocity) velocity.x = maxVelocity;
	velocity.y += dy;
	if (velocity.y > maxVelocity) velocity.y = maxVelocity;
	velocity.z += dz;
	if (velocity.z > maxVelocity) velocity.z = maxVelocity;
}

void Player::Decelerate(float d)
{
	if (velocity.x > 0) {
		velocity.x -= d;
		if (velocity.x < 0) velocity.x = 0;
	}
	if(velocity.x < 0){
		velocity.x += d;
		if (velocity.x > 0) velocity.x = 0;
	}
	if (velocity.y > 0) {
		velocity.y -= d;
		if (velocity.y < 0) velocity.y = 0;
	}
	if (velocity.y < 0) {
		velocity.y += d;
		if (velocity.y > 0) velocity.y = 0;
	}
	if (velocity.z > 0) {
		velocity.z -= d;
		if (velocity.z < 0) velocity.z = 0;
	}
	if (velocity.z < 0) {
		velocity.z += d;
		if (velocity.z > 0) velocity.z = 0;
	}
}

PointLight* Player::GetLeftEngine()
{
	return leftEngine;
}

PointLight* Player::GetRightEngine()
{
	return rightEngine;
}