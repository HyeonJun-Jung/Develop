#pragma once
#include "Object.h"

class Text :
    public Object
{
	struct Sentance {
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	}; 
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 textcoord;
	};

public:
    Text();

public:
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*);
	bool AddText(ID3D11Device* dev, char* text, float positionX, float positionY);
	bool AddTextWithValue(ID3D11Device* dev, char* text, int* value, float positionX, float positionY);
	bool AddTextWithValue(ID3D11Device* dev, char* text, float* value, float positionX, float positionY);
	virtual bool Update(ID3D11DeviceContext* devcon, float fDeltaTime);
	virtual bool Render(ID3D11DeviceContext* devcon);
	virtual bool PostRender(ID3D11DeviceContext* devcon);

private:
	bool InitializeText(ID3D11Device* dev, char* text, float positionX, float positionY);
	bool InitializeTextWithValue(ID3D11Device* dev, char* text, int* value, float positionX, float positionY);
	bool InitializeTextWithValue(ID3D11Device* dev, char* text, float* value, float positionX, float positionY);

private:
	class FontClass* m_Font = nullptr;
	class FontShaderClass* m_TextShader = nullptr;

	vector<Sentance*> m_TextList;
	vector<Sentance*> m_TextwithValueList;
	
};

