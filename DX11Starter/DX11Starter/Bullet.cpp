#include "Bullet.h"


const float Bullet::speed = 30.0f;
const float Bullet::range = 50.0f;
const float Bullet::lifetime = range / speed;
const float Bullet::laserRadius = 0.1f;

Bullet::Bullet(Mesh* mesh, Material* material) : Entity(mesh, material)
{
	this->SetScale(0.15f, 0.15f, 0.15f);
	this->laser = new PointLight();
	this->laser->AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	this->laser->DiffuseColor = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	this->laser->SpecularColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	this->laser->Position = this->GetPosition();
	this->laser->Radius = Bullet::laserRadius;
	this->SetActive(true);
}

Bullet::~Bullet()
{
	delete laser;
}

void Bullet::Update(float deltaTime, float totalTime)
{
	if (this->IsActive() != true)
	{
		return;
	}

	if (totalTime >= spawnTime + lifetime)
	{
		this->SetActive(false);
		this->laser->Radius = 0.0f;
	}

	this->Move(0.0f, 0.0f, speed * deltaTime);
	this->laser->Position = this->GetPosition();
}

void Bullet::Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager)
{
	if (this->IsActive() != true)
	{
		return;
	}

	SimpleVertexShader* vShader = GetMaterial()->GetVertexShader();
	SimplePixelShader* pShader = GetMaterial()->GetPixelShader();

	vShader->SetShader();
	pShader->SetShader();

	// Send data to shader variables
	vShader->SetMatrix4x4("world", GetWorld());
	vShader->SetMatrix4x4("view", camera->GetView());
	vShader->SetMatrix4x4("projection", camera->GetProj());
	vShader->SetMatrix4x4("normalWorld", GetNormalWorld());

	pShader->SetData("bulletLight", &(*laser), sizeof(PointLight));
	pShader->SetData("cameraPosition", &(camera->GetCamPosition()), sizeof(XMFLOAT3));

	vShader->CopyAllBufferData();
	pShader->CopyAllBufferData();

	Entity::Draw(context, camera, lightManager);
}

void Bullet::Collides()
{
	this->SetActive(false);
	this->laser->Radius = 0.0f;
}

PointLight* Bullet::GetLaser()
{
	return laser;
}

//Spawns the bullet and turns it on
void Bullet::Launch(float timeStamp)
{
	this->SetPosition(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z + player->GetRadius());
	this->spawnTime = timeStamp;
	this->SetActive(true);
	this->laser->Radius = Bullet::laserRadius;
	this->laser->Position = this->GetPosition();
}

//Gives the bullet a reference to the player so it knows where to spawn
void Bullet::Link(Entity* player)
{
	this->player = player;
}