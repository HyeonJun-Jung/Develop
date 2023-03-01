#include "BitmapClass.h"
#include "..\stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Filename: bitmapclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "bitmapclass.h"

BitmapClass::BitmapClass()
{
}


BitmapClass::BitmapClass(const BitmapClass& other)
{
}


BitmapClass::~BitmapClass()
{
}


bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename,
							int bitmapWidth, int bitmapHeight)
{
	bool result;

	// 화면 크기를 저장합니다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 렌더링 할 비트맵의 크기를 저장합니다.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// 이전 렌더링의 위치를 음수로 초기화합니다.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// 정점 및 인덱스 버퍼를 초기화합니다.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// 이 모델의 텍스처를 로드합니다.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	// 모델 텍스처를 반환합니다..
	ReleaseTexture();

	// 정점 및 인덱스 버퍼를 종료합니다.
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;


	// 화면의 다른 위치로 렌더링하기 위해 동적 정점 버퍼를 다시 빌드합니다.
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}

	// 그리기를 준비하기 위해 정점 및 인덱스 버퍼를 그래픽 파이프 라인에 놓습니다.
	RenderBuffers(deviceContext);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	HRESULT result;

	m_indexCount = m_vertexCount = 6;

	//정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	//인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	// 정점 배열을 0으로 초기화합니다.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 데이터로 인덱스 배열을 초기화합니다.
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}
	

	//정점 버퍼의 description을 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼를 생성합니다.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 인덱스 버퍼의 description을 작성합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼를 해제합니다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// 비트맵을 렌더링할 위치가 변경되지 않은 경우 정점 버프를 업데이트 하지않는다.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// 비트맵 왼쪽 화면 좌표
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// 비트맵 오른쪽 화면 좌표
	right = left + (float)m_bitmapWidth;

	// 비트맵 위 화면 좌표
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// 비트맵 아래 화면 좌표
	bottom = top - (float)m_bitmapHeight;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 정점 배열에 데이터를 로드합니다.
	// 첫번째 삼각형
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두번째 삼각형
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 정점 버퍼를 쓸 수 있도록 잠급니다.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 얻습니다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// 정점 버퍼에 데이터를 복사합니다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// 정점 버퍼의 잠금을 해제합니다.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 더 이상 필요없는 정점 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	stride = sizeof(VertexType);
	offset = 0;

	// input assembler에 정점 버퍼를 활성화하여 그려질 수 있게 합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// input assembler에 인덱스 버퍼를 활성화하여 그려질 수 있게 합니다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형을 설정합니다. 여기서는 삼각형입니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//삼각형 D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	//사각형 D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* Filename)
{
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	return m_Texture->Initialize(device, Filename);
}

void BitmapClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}
