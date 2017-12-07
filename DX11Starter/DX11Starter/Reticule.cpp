#include "Reticule.h"



Reticule::Reticule(Mesh* mesh, Material* material, Player* player, TargetManager* targetManager, ID3D11BlendState* blend, float bulletRad) : Entity(mesh, material)
{
	this->playerRef = player;
	this->targetRef = targetManager;
	this->blend = blend;
	this->bulletRadSq = pow(bulletRad, 2.0f);
	this->SetRotation(0.0f, XM_PI, 0.0f);
}


Reticule::~Reticule()
{
	blend->Release();
}

void Reticule::Update(float deltaTime, float totalTime)
{
	XMFLOAT3 pos = playerRef->GetPosition();

	//Build list of targets in the path of the player
	vector<Entity*> inRange = vector<Entity*>();
	for each (Entity* t in targetRef->GetTargets())
	{
		//Check active
		if (!t->IsActive()) continue;

		XMFLOAT3 tPos = t->GetPosition();

		//Check it's not too far away or behind us
		if (tPos.z > pos.z + Bullet::range || pos.z > tPos.z) continue;

		//Check it intersects path of bullet
		float distsq = pow(tPos.x - pos.x, 2.0f) + pow(tPos.y - pos.y, 2.0f);
		if (distsq > bulletRadSq + pow(t->GetRadius(), 2.0f) * 2.0f) continue;

		inRange.push_back(t);
	}

	if (inRange.size() > 0)
	{
		//Find nearest target
		size_t closestIndex = 0;
		float closest = pos.z + Bullet::range;
		for (size_t i = 0; i < inRange.size(); i++)
		{
			float range = inRange.at(i)->GetPosition().z;
			if (range < closest)
			{
				closest = range;
				closestIndex = i;
			}
		}

		this->SetPosition(pos.x, pos.y, closest - inRange.at(closestIndex)->GetRadius());
	}
	else
	{
		//Put reticule at max range
		this->SetPosition(pos.x, pos.y, pos.z + Bullet::range);
	}

}

void Reticule::Draw(ID3D11DeviceContext* context, Camera* camera, LightManager* lightManager)
{
	SimpleVertexShader* vShader = GetMaterial()->GetVertexShader();
	SimplePixelShader* pShader = GetMaterial()->GetPixelShader();

	vShader->SetShader();
	pShader->SetShader();

	vShader->SetMatrix4x4("world", GetWorld());
	vShader->SetMatrix4x4("view", camera->GetView());
	vShader->SetMatrix4x4("projection", camera->GetProj());

	pShader->SetShaderResourceView("diffuseTexture", GetMaterial()->GetTexture());
	pShader->SetSamplerState("basicSampler", GetMaterial()->GetSampler());

	vShader->CopyAllBufferData();
	pShader->CopyAllBufferData();

	Entity::Draw(context, camera, lightManager);
}

ID3D11BlendState* Reticule::GetBlend()
{
	return blend;
}