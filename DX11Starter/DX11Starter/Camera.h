#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class Camera
{
public:
	Camera(float width, float height, float fov, float nearClip, float farClip);
	~Camera();

	void Update(float deltaTime, float totalTime, XMFLOAT3 playerPosition);
	void Rotate(float dx, float dy);
	void Resize(float width, float height);
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y);

	XMFLOAT4X4 GetView();
	XMFLOAT4X4 GetProj();
	XMFLOAT3 GetCamPosition();
private:
	//Matrixes
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;

	//Look-to data
	XMFLOAT3 camPosition;
	float yRotation;
	float xRotation;

	//Proj data
	float width;
	float height;
	float fov;
	float nearClip;
	float farClip;

	//Camera stats
	const float speed = 1.75f;
	const float spinRate = XM_PI * 0.002f;

	//Private methods
	void RecalcProj();
};

