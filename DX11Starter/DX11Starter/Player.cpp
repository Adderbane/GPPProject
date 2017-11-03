#include "Player.h"

using namespace std;

Player::Player(Mesh* mesh, Material* material) : Entity(mesh, material)
{
	this->SetActive(true);
	this->SetRotation(XM_PIDIV2, 0.0f, 0.0f);
	decelRate = 0.5;
	xCap = 4.0f;
	yCap = 2.0f;
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
	
}

//void Player::Draw()
//{
//}

void Player::Collides()
{
}
