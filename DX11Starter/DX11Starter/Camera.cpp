#include "Camera.h"
#include <string>

Camera::Camera(float width, float height, float fov, float nearClip, float farClip)
{
	xRotation = 0.0f;
	yRotation = 0.0f;
	XMStoreFloat3(&camPosition, XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f));

	this->width = width;
	this->height = height;
	this->fov = fov;
	this->nearClip = nearClip;
	this->farClip = farClip;

	RecalcProj();
}

Camera::~Camera()
{
}

//Public methods
void Camera::Update(float deltaTime, float totalTime, XMFLOAT3 playerPosition)
{
	//Get direction
	XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(xRotation, yRotation, 0.0f);
	XMVECTOR camInPlayerPlane = XMLoadFloat3(&camPosition);
	camInPlayerPlane = XMVectorAdd(camInPlayerPlane, XMVectorSet(0.0f, 0.0f, 4.0f, 0.0f));
	XMVECTOR playerDirection = XMLoadFloat3(&playerPosition) - camInPlayerPlane;
	
	XMVECTOR lookDir = XMVector4Normalize(XMVectorAdd(XMVectorScale(playerDirection, 0.2), camInPlayerPlane) - XMLoadFloat3(&camPosition));
	XMVECTOR upDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR position = XMLoadFloat3(&camPosition);

	//Apply camera movement
	if (GetAsyncKeyState('W') & 0x8000)
	{
		position = XMVectorAdd(position, lookDir * speed * deltaTime);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		position = XMVectorAdd(position, -1.0f * lookDir * speed * deltaTime);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		position = XMVectorAdd(position, XMVector3Cross(lookDir, upDir) * speed * deltaTime);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		position = XMVectorAdd(position, -1.0f * XMVector3Cross(lookDir, upDir) * speed * deltaTime);
	}
	if (GetAsyncKeyState('Z') & 0x8000)
	{
		position = XMVectorAdd(position, upDir * speed * deltaTime);
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		position = XMVectorAdd(position, -1.0f * upDir * speed * deltaTime);
	}

	//Move camera forward
	
	//Get view matrix
	XMMATRIX view = XMMatrixLookToLH(position, lookDir, upDir);

	//Save data
	XMStoreFloat3(&camPosition, position);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
}

void Camera::Rotate(float dx, float dy)
{
	yRotation += dx * spinRate;
	xRotation += dy * spinRate;

	//Clamp xRotation
	if (xRotation > XM_PIDIV2)
	{
		xRotation = XM_PIDIV2;
	}
	if (xRotation < -1.0f * XM_PIDIV2)
	{
		xRotation = -XM_PIDIV2;
	}

	//Simplify yRotation
	if (yRotation > XM_PI)
	{
		yRotation = yRotation - (XM_PI * 2.0f);
	}
	if (yRotation < -1.0f * XM_PI)
	{
		yRotation = yRotation + (XM_PI * 2.0f);
	}
}

void Camera::Resize(float width, float height)
{
	this->width = width;
	this->height = height;

	RecalcProj();
}

void Camera::SetPosition(float x, float y, float z)
{
	camPosition = XMFLOAT3(x, y, z);
}

void Camera::SetRotation(float x, float y)
{
	xRotation = x;
	yRotation = y;
}

//Getters
XMFLOAT4X4 Camera::GetView()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProj()
{
	return projMatrix;
}

XMFLOAT3 Camera::GetCamPosition()
{
	return camPosition;
}

//Private methods
void Camera::RecalcProj()
{
	// Create the Projection matrix
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		fov,						// Field of View Angle
		(float)width / height,		// Aspect ratio
		nearClip,					// Near clip plane distance
		farClip);					// Far clip plane distance
	XMStoreFloat4x4(&projMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}