#include "Player.h"



Player::Player(Mesh* mesh, Material* material) : Entity(mesh, material)
{
	this->SetActive(true);
	decelRate = 0.5;
}


Player::~Player()
{
}

void Player::Update(float deltaTime, float totalTime)
{
	Entity::Update(deltaTime, totalTime);
	this->Move(0, 0, 0.1 * deltaTime);
}

void Player::Draw()
{
}

void Player::Collides()
{
}
