#pragma once

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "fontclass.h"
#include "fontshaderclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX);

	bool SetMousePosition(int, int, ID3D11DeviceContext*);

	bool SetFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);

	bool SetCameraPosition(float, float, float, ID3D11DeviceContext*);
	bool SetCameraLookAt(float, float, float, ID3D11DeviceContext*);

	bool SetRenderCount(int RenderCount, ID3D11DeviceContext*);


private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, XMMATRIX, XMMATRIX);

private:
	FontClass* m_Font = nullptr;
	FontShaderClass* m_FontShader = nullptr;
	int m_screenWidth = 0;
	int m_screenHeight = 0;
	XMMATRIX m_baseViewMatrix;


	// 이 튜토리얼에서는 두 개의 문장을 사용할 것입니다.
	SentenceType* m_sentence1;
	SentenceType* m_sentence2;
	SentenceType* m_CameraPositionX;
	SentenceType* m_CameraPositionY;
	SentenceType* m_CameraPositionZ;
	SentenceType* m_CameraLookAtX;
	SentenceType* m_CameraLookAtY;
	SentenceType* m_CameraLookAtZ;
};
