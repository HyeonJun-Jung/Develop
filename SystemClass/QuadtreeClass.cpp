#include "QuadtreeClass.h"
#include "TerrainClass.h"
#include "FrustumClass.h"
#include "TerrainShaderClass.h"

QuadtreeClass::QuadtreeClass()
{
	m_triangleCount = 0;
	m_drawnCount = 0;
}

QuadtreeClass::QuadtreeClass(const QuadtreeClass&)
{
}

QuadtreeClass::~QuadtreeClass()
{
}

bool QuadtreeClass::Initialize(TerrainClass* terrain, ID3D11Device* device)
{

	// Terrain���κ��� ���� ������ �޾ƿ´�.
	int vertexCount = terrain->GetVertexCount();

	m_triangleCount = vertexCount / 3;

	// VertexList ����
	m_VertexList = new VertexType[vertexCount];
	if (!m_VertexList)
		return false;

	// Terrain���κ��� ���� ������ �����ؿ´�
	terrain->CopyVertexArray((void *)m_VertexList);

	float centerX = 0;
	float centerZ = 0;
	float width = 0;

	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	m_RootNode = new NodeType;
	if (!m_RootNode)
		return false;

	CreateQuadtree(device, m_RootNode,  centerX, centerZ, width);


	// ����Ʈ������ ���ȿ� ���� ���۵��� ��� �����Ǿ��� ������ VertexList�� �����Ѵ�.
	if (m_VertexList)
	{
		delete [] m_VertexList;
		m_VertexList = 0;
	}

    return true;
}

void QuadtreeClass::Shutdown()
{
	if (m_RootNode)
	{
		ReleaseNode(m_RootNode);
		delete m_RootNode;
		m_RootNode = 0;
	}
}

bool QuadtreeClass::Render(FrustumClass* frustum, TerrainShaderClass* Shader, ID3D11DeviceContext* devcon)
{
	m_drawnCount = 0;
	RenderNode(m_RootNode, frustum, devcon, Shader);
    return true;
}

void QuadtreeClass::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& width)
{
	// �߰��� ã��
	centerX = 0;
	centerZ = 0; 

	for (int i = 0; i < vertexCount; i++)
	{
		centerX += m_VertexList[i].Position.x;
		centerZ += m_VertexList[i].Position.z;
	}

	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// width ���
		// �޽��� �ִ� �� �ּ� ũ�⸦ �ʱ�ȭ�մϴ�.
	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	float minWidth = fabsf(m_VertexList[0].Position.x - centerX);
	float minDepth = fabsf(m_VertexList[0].Position.z - centerZ);

	// ��� ������ ���캸�� �޽��� �ִ� �ʺ�� �ּ� �ʺ�� ���̸� ã���ϴ�.
	for (int i = 0; i < vertexCount; i++)
	{
		float width = fabsf(m_VertexList[i].Position.x - centerX);
		float depth = fabsf(m_VertexList[i].Position.z - centerZ);

		if (width > maxWidth) { maxWidth = width; }
		if (depth > maxDepth) { maxDepth = depth; }
		if (width < minWidth) { minWidth = width; }
		if (depth < minDepth) { minDepth = depth; }
	}

	// �ּҿ� �ִ� ���̿� �ʺ� ������ ���� �ִ� ���� ã���ϴ�.
	float maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	// �޽��� �ִ� ������ ����մϴ�.
	width = max(maxX, maxZ) * 2.0f;

}

void QuadtreeClass::CreateQuadtree(ID3D11Device* device, NodeType* Node, float PositionX, float PositionZ, float width)
{
	// ����� ��ġ�� ũ�⸦ �����Ѵ�.
	Node->positionX = PositionX;
	Node->positionZ = PositionZ;
	Node->width = width;

	// ����� �ﰢ�� ���� 0���� �ʱ�ȭ�մϴ�.
	Node->triangleCount = 0;

	//���� �� �ε��� ���۸� null�� �ʱ�ȭ�մϴ�.
	Node->vertexBuffer = 0;
	Node->indexBuffer = 0;

	// �� ����� �ڽ� ��带 null�� �ʱ�ȭ�մϴ�.
	Node->Child[0] = 0;
	Node->Child[1] = 0;
	Node->Child[2] = 0;
	Node->Child[3] = 0;

	// �� ��� �ȿ� �ִ� �ﰢ�� ���� ����.
	int numTriangles = CountTriangles(PositionX, PositionZ, width);

	// ��� 1: �� ��忡 �ﰢ���� ������ ����ִ� ���·� ���ư��� ó���� �ʿ䰡 �����ϴ�.
	if (numTriangles == 0)
	{
		return;
	}

	// ��� 2: �� ��忡 �ʹ� ���� �ﰢ���� �ִ� ��� 4 ���� ������ ũ���� �� ���� Ʈ�� ���� �����մϴ�.
	if (numTriangles > MAX_TRIANGLES)
	{
		for (int i = 0; i < 4; i++)
		{
			// ���ο� �ڽ� ��忡 ���� ��ġ �������� ����մϴ�.
			float offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			// �� ��忡 �ﰢ���� �ִ��� Ȯ���մϴ�.
			int count = CountTriangles((PositionX + offsetX), (PositionZ + offsetZ), (width / 2.0f));
			if (count > 0)
			{
				// �� �� ��尡�ִ� �ﰢ�����ִ� ��� �ڽ� ��带 ����ϴ�.
				Node->Child[i] = new NodeType;

				// ������ �� �ڽ� ��忡�� �����ϴ� Ʈ���� Ȯ���մϴ�.
				CreateQuadtree(device, Node->Child[i], (PositionX + offsetX), (PositionZ + offsetZ), (width / 2.0f));
			}
		}
		return;
	}

	// ��� 3: �� ��尡 ��� �����ʰ� �� ����� �ﰢ�� ���� �ִ� ������ ������ 
	// �� ���� Ʈ���� �� �Ʒ��� �����Ƿ� ������ �ﰢ�� ����� ����ϴ�.
	Node->triangleCount = numTriangles;

	// ������ ���� ����մϴ�.
	int vertexCount = numTriangles * 3;

	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[vertexCount];

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[vertexCount];

	// �� ���ο� ���� �� �ε��� �迭�� �ε����� �ʱ�ȭ�մϴ�.
	int index = 0;
	int vertexIndex = 0;

	// ���� ����� ��� �ﰢ���� ���� ���ϴ�.
	for (int i = 0; i < m_triangleCount; i++)
	{
		// �ﰢ������ ��� �ȿ� ������ ������ �迭�� �߰��մϴ�.
		if (IsTriangleContained(i, PositionX, PositionZ, width) == true)
		{
			// ���� ���ؽ� ��Ͽ� �ε����� ����մϴ�.
			vertexIndex = i * 3;

			// ���� ��Ͽ��� �� �ﰢ���� �� �������� �����ɴϴ�.
			vertices[index].Position = m_VertexList[vertexIndex].Position;
			vertices[index].Texcoord = m_VertexList[vertexIndex].Texcoord;
			vertices[index].Normal = m_VertexList[vertexIndex].Normal;
			indices[index] = index;
			index++;

			vertexIndex++;
			vertices[index].Position = m_VertexList[vertexIndex].Position;
			vertices[index].Texcoord = m_VertexList[vertexIndex].Texcoord;
			vertices[index].Normal = m_VertexList[vertexIndex].Normal;
			indices[index] = index;
			index++;

			vertexIndex++;
			vertices[index].Position = m_VertexList[vertexIndex].Position;
			vertices[index].Texcoord = m_VertexList[vertexIndex].Texcoord;
			vertices[index].Normal = m_VertexList[vertexIndex].Normal;
			indices[index] = index;
			index++;
		}
	}

	// ���� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ��ħ�� ���� ���۸� ����ϴ�.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &Node->vertexBuffer);

	// �ε��� ������ ������ �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� ����ϴ�.
	device->CreateBuffer(&indexBufferDesc, &indexData, &Node->indexBuffer);

	// ���� ����� ���ۿ� �����Ͱ� ����ǹǷ� �������� �ε��� �迭�� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	
}

int QuadtreeClass::CountTriangles(float positionX, float positionZ, float width)
{
	// ī��Ʈ�� 0���� �ʱ�ȭ�Ѵ�.
	int count = 0;

	// ��ü �޽��� ��� �ﰢ���� ���캸�� � ��尡 �� ��� �ȿ� �־�� �ϴ��� Ȯ���մϴ�.
	for (int i = 0; i < m_triangleCount; i++)
	{
		// �ﰢ���� ��� �ȿ� ������ 1�� ������ŵ�ϴ�.
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
		{
			count++;
		}
	}

	return count;
}

bool QuadtreeClass::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
	// �� ����� �ݰ��� ����մϴ�.
	float radius = width / 2.0f;

	// �ε����� ���� ������� �����ɴϴ�.
	int vertexIndex = index * 3;

	// ���� ��Ͽ��� �� �ﰢ���� �� �������� �����ɴϴ�.
	float x1 = m_VertexList[vertexIndex].Position.x;
	float z1 = m_VertexList[vertexIndex].Position.z;
	vertexIndex++;

	float x2 = m_VertexList[vertexIndex].Position.x;
	float z2 = m_VertexList[vertexIndex].Position.z;
	vertexIndex++;

	float x3 = m_VertexList[vertexIndex].Position.x;
	float z3 = m_VertexList[vertexIndex].Position.z;

	// �ﰢ���� x ��ǥ�� �ּҰ��� ��� �ȿ� �ִ��� Ȯ���Ͻʽÿ�.
	float minimumX = min(x1, min(x2, x3));
	if (minimumX > (positionX + radius))
	{
		return false;
	}

	// �ﰢ���� x ��ǥ�� �ִ� ���� ��� �ȿ� �ִ��� Ȯ���Ͻʽÿ�.
	float maximumX = max(x1, max(x2, x3));
	if (maximumX < (positionX - radius))
	{
		return false;
	}

	// �ﰢ���� z ��ǥ�� �ּҰ��� ��� �ȿ� �ִ��� Ȯ���Ͻʽÿ�.
	float minimumZ = min(z1, min(z2, z3));
	if (minimumZ > (positionZ + radius))
	{
		return false;
	}

	// �ﰢ���� z ��ǥ�� �ִ� ���� ��� �ȿ� �ִ��� Ȯ���Ͻʽÿ�.
	float maximumZ = max(z1, max(z2, z3));
	if (maximumZ < (positionZ - radius))
	{
		return false;
	}

	return true;
}

void QuadtreeClass::RenderNode(NodeType* node, FrustumClass* frustum, ID3D11DeviceContext* devcon, TerrainShaderClass* shader)
{
	// ���� ��尡 ���̴��� Frustum�� ���� �Ǵ�
	if (!frustum->CheckCube(node->positionX, 0, node->positionZ, node->width))
		return;

	// �ڽ� ��尡 �ִ� ��� �ڽ� ��忡 ���� RenderNode
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (node->Child[i] != 0)
		{
			RenderNode(node->Child[i], frustum, devcon, shader);
			count++;
		}
	}

	// �ڽ� ��尡 �ִ� ��� �θ�� Render �� �ʿ䰡 ����.
	if (count != 0)
	{
		return;
	}

	// �ڽ� ��尡 ���� ��� Render

	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	devcon->IASetVertexBuffers(0, 1, &node->vertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	devcon->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ������ ���ۿ��� ������ �Ǿ�� �ϴ� ������Ƽ�� ������ �����մϴ�. �� ��쿡�� �ﰢ���Դϴ�.
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �� ��忡�� �ε����� ���� �����մϴ�.
	int indexCount = node->triangleCount * 3;

	// ���� ���̴��� ȣ���Ͽ� �� ����� �ٰ����� �������մϴ�.
	shader->RenderShader(devcon, indexCount);

	// �� ������ ���� ������ �� �������� ���� �ø��ϴ�.
	m_drawnCount += node->triangleCount;

}

void QuadtreeClass::ReleaseNode(NodeType* ParentNode)
{
	for (int i = 0; i < 4; i++)
	{
		if (ParentNode->Child[i] != 0)
			ReleaseNode(ParentNode->Child[i]);
	}

	
	if (ParentNode->vertexBuffer)
	{
		ParentNode->vertexBuffer->Release();
		ParentNode->vertexBuffer = 0;
	}

	if (ParentNode->indexBuffer)
	{
		ParentNode->indexBuffer->Release();
		ParentNode->indexBuffer = 0;
	}
}
