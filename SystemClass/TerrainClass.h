#pragma once
#include "..\stdafx.h"
#include "StaticModel.h"

const int TEXTURE_REPEAT = 8;
class TextureClass;

class TerrainClass : StaticModel
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texcoords;
		XMFLOAT3 normal;
	};

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D11Device*, const char*, const WCHAR*);
	void shutdown();
	bool Render();

	int GetVertexCount();
	int GetIndexCount();
	ID3D11Buffer* GetVertexBuffer() { return m_vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_indexBuffer; }
	void CopyVertexArray(void*);
	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadHeightMap(const char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();
	void CalculateTextureCoord();


	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	int m_terrainWidth = 0;
	int m_terrainHeight = 0;
	int m_VertexCount = 0;
	int m_IndexCount = 0;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	HeightMapType* m_heightMap = nullptr;
	TextureClass* m_Texture = nullptr;
	VertexType* m_Vertices = nullptr;
};

