#include "SkydomeClass.h"
#include "ShaderManager.h"
#include "D3DClass.h"
#include "CameraClass.h"
SkydomeClass::SkydomeClass()
{
}

SkydomeClass::SkydomeClass(const SkydomeClass&)
{
}

SkydomeClass::~SkydomeClass()
{
}

bool SkydomeClass::Initialize(ID3D11Device* dev)
{
	const char * SkydoomFileName = "../SystemClass/data/Terrain/skydome.txt";	

	m_RT = RT_SkyDome;

	if (!LoadSkydoom(SkydoomFileName))
		return false;

	if (!InitializeBuffer(dev))
		return false;

	return true;
}

bool SkydomeClass::Render()
{
	if (!DynamicModel::Render())
		return false;

	XMFLOAT3 CameraPos = CameraClass::GetSingleton()->GetPosition();
	XMMATRIX WorldMatrix = XMMatrixTranslation(CameraPos.x, CameraPos.y, CameraPos.z);
	
	XMMATRIX ViewMatrix; CameraClass::GetSingleton()->GetViewMatrix(ViewMatrix);
	XMMATRIX ProjectionMatrix; D3DClass::GetSingleton()->GetProjectionMatrix(ProjectionMatrix);
	ShaderManager::GetSingleton()->SetMatrixBuffer(WorldMatrix, ViewMatrix, ProjectionMatrix);
	
	//ShaderManager::GetSingleton()->SetWorldMatrix(WorldMatrix);
	ShaderManager::GetSingleton()->SetGradientBuffer(m_ApexColor, m_CenterColor);

	D3DClass::GetSingleton()->TurnZBufferOff();
	D3DClass::GetSingleton()->TurnOffCulling();

	RenderBuffer(D3DClass::GetSingleton()->GetDeviceContext());

	D3DClass::GetSingleton()->GetDeviceContext()->DrawIndexed(m_IndexCount, 0, 0);

	D3DClass::GetSingleton()->TurnZBufferOn();
	D3DClass::GetSingleton()->TurnOnCulling();


	WorldMatrix = XMMatrixIdentity();
	ShaderManager::GetSingleton()->SetMatrixBuffer(WorldMatrix, ViewMatrix, ProjectionMatrix);

	return true;
}

bool SkydomeClass::Update(float fDeltaTime)
{
	if (!DynamicModel::Update(fDeltaTime))
		return false;

	return true;
}


void SkydomeClass::ShutDown()
{	
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = 0;
	}

	if (m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = 0;
	}
}

bool SkydomeClass::LoadSkydoom(const char* SkydoomFileName)
{
	// �� ������ ���ϴ�.
	ifstream fin;
	fin.open(SkydoomFileName);

	// ������ �� �� ������ �����մϴ�.
	if (fin.fail())
	{
		return false;
	}

	// ���ؽ� ī��Ʈ�� ������ �д´�.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// ���ؽ� ī��Ʈ�� �д´�.
	fin >> m_VertexCount;

	// �ε����� ���� ���� ���� ���� �����մϴ�.
	m_IndexCount = m_VertexCount;

	// �о� ���� ���� ������ ����Ͽ� ���� ����ϴ�.
	m_model = new ModelType[m_VertexCount];
	if (!m_model)
	{
		return false;
	}

	// �������� ���� �κб��� �д´�.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// ���ؽ� �����͸� �н��ϴ�.
	for (int i = 0; i < m_VertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// �� ������ �ݴ´�.
	fin.close();

	return true;
}

bool SkydomeClass::InitializeBuffer(ID3D11Device* device)
{
	VertexType* vertices = new VertexType[m_VertexCount];
	if (!vertices)
		return false;

	unsigned long* indices = new unsigned long[m_IndexCount];
	if (!indices)
		return false;

	for (int i = 0; i < m_VertexCount; i++)
	{
		vertices[i].Position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		indices[i] = i;
	}

	D3D11_BUFFER_DESC VertexBufferDesc;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&VertexBufferDesc, &vertexData, &m_VertexBuffer)))
	{
		return false;
	}

	D3D11_BUFFER_DESC IndexBufferDesc;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&IndexBufferDesc, &indexData, &m_IndexBuffer)))
	{
		return false;
	}

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void SkydomeClass::RenderBuffer(ID3D11DeviceContext* devcon)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	devcon->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	devcon->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}


int SkydomeClass::GetIndexCount()
{
	return m_IndexCount;
}

XMFLOAT4 SkydomeClass::GetApexColor()
{
	return m_ApexColor;
}

XMFLOAT4 SkydomeClass::GetCenterColor()
{
	return m_CenterColor;
}
