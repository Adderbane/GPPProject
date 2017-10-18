#pragma once

#include <d3d11.h>
#include <vector>
#include <fstream>
#include <DirectXMath.h>
#include "Vertex.h"

using namespace DirectX;

class Mesh
{
public:
	Mesh(Vertex verticies[], int vertexCount, unsigned int indices[], int indexCount, ID3D11Device* device);
	Mesh(char* filename, ID3D11Device* device);
	~Mesh();

	//Getters
	ID3D11Buffer* const* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();
private:
	//Buffer Data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int indexCount;

	//Helpers
	void CreateBuffers(Vertex verticies[], int vertexCount, unsigned int indices[], int indexCount, ID3D11Device* device);
};

