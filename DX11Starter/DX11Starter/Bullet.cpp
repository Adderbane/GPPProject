#include "Bullet.h"


const float Bullet::speed = 30.0f;
const float Bullet::range = 50.0f;
const float Bullet::lifetime = range / speed;

Bullet::Bullet(Mesh* mesh, Material* material) : Entity(mesh, material)
{
	this->SetScale(0.2f, 0.2f, 0.2f);
	this->SetActive(true);
}

Bullet::~Bullet()
{

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
	}

	this->Move(0.0f, 0.0f, speed * deltaTime);
}

void Bullet::Draw()
{
	if (this->IsActive() != true)
	{
		return;
	}
}

void Bullet::Collides()
{
	this->SetActive(false);
}

//Spawns the bullet and turns it on
void Bullet::Launch(float timeStamp)
{
	this->SetPosition(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z);
	this->spawnTime = timeStamp;
	this->SetActive(true);
}

//Gives the bullet a reference to the player so it knows where to spawn
void Bullet::Link(Entity* player)
{
	this->player = player;
}