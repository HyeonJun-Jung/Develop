#pragma once
#include "..\stdafx.h"
#include "DynamicModel.h"
class SkydomeClass : public DynamicModel
{
	struct ModelType {
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	
	struct VertexType {
		XMFLOAT3 Position;
	};

public:
	SkydomeClass();
	SkydomeClass(const SkydomeClass&);
	~SkydomeClass();

	bool Initialize(ID3D11Device* dev);
	bool Render();
	bool Update(float fDeltaTime);
	void ShutDown();

	int GetIndexCount();
	XMFLOAT4 GetApexColor();
	XMFLOAT4 GetCenterColor();

private:
	bool LoadSkydoom(const char* SkydoomFileName);
	bool InitializeBuffer(ID3D11Device* device);
	void RenderBuffer(ID3D11DeviceContext* devcon);


private:
	ModelType* m_model = nullptr;

	int m_VertexCount = 0;
	int m_IndexCount = 0;

	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;

	XMFLOAT4 m_ApexColor = XMFLOAT4(0.6f, 1.0f, 1.0f, 1.0f);
	// XMFLOAT4(0.0f, 0.15f, 0.66f, 1.0f)
	XMFLOAT4 m_CenterColor = XMFLOAT4(0.8f, 1.f, 1.0f, 1.0f);
};

