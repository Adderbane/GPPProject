#include "Player.h"

using namespace std;

Player::Player(Mesh* mesh, Material* material) : Entity(mesh, material)
{
	this->SetActive(true);
	this->SetRotation(XM_PIDIV2, 0.0f, 0.0f);
	decelRate = 0.5;
}


Player::~Player()
{
}

void Player::Update(float deltaTime, float totalTime)
{
	Entity::Update(deltaTime, totalTime);
	this->Move(0, 0, 0.1f * deltaTime);
}

void Player::Draw()
{
}

void Player::Collides()
{
}
