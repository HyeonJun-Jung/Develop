#pragma once
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class TextureClass;
class ModelLoader;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(HWND, ID3D11Device*, ID3D11DeviceContext*, char*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadModelFromFbx(HWND hwnd, ID3D11Device* dev, ID3D11DeviceContext* devcon, char* filename);
	bool LoadModelFromtxt(char*);
	void ReleaseModel();
	bool LoadTexture(ID3D11Device*, WCHAR*);	
	void ReleaseTexture();

	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetTranslate(float x, float y, float z);


private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	ModelType* m_model = nullptr;
	TextureClass* m_Texture = nullptr;
	ModelLoader* m_ModelLoader = nullptr;

	XMMATRIX m_RotationMatrix;
	XMMATRIX m_ScaleMatrix;
	XMMATRIX m_TranslateMatrix;
};

#endif