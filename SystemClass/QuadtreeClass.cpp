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

	// Terrain으로부터 정점 개수를 받아온다.
	int vertexCount = terrain->GetVertexCount();

	m_triangleCount = vertexCount / 3;

	// VertexList 생성
	m_VertexList = new VertexType[vertexCount];
	if (!m_VertexList)
		return false;

	// Terrain으로부터 정점 정보를 복사해온다
	terrain->CopyVertexArray((void *)m_VertexList);

	float centerX = 0;
	float centerZ = 0;
	float width = 0;

	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	m_RootNode = new NodeType;
	if (!m_RootNode)
		return false;

	CreateQuadtree(device, m_RootNode,  centerX, centerZ, width);


	// 쿼드트리에서 노드안에 정점 버퍼들이 모두 생성되었기 때문에 VertexList를 해제한다.
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
	// 중간점 찾기
	centerX = 0;
	centerZ = 0; 

	for (int i = 0; i < vertexCount; i++)
	{
		centerX += m_VertexList[i].Position.x;
		centerZ += m_VertexList[i].Position.z;
	}

	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// width 계산
		// 메쉬의 최대 및 최소 크기를 초기화합니다.
	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	float minWidth = fabsf(m_VertexList[0].Position.x - centerX);
	float minDepth = fabsf(m_VertexList[0].Position.z - centerZ);

	// 모든 정점을 살펴보고 메쉬의 최대 너비와 최소 너비와 깊이를 찾습니다.
	for (int i = 0; i < vertexCount; i++)
	{
		float width = fabsf(m_VertexList[i].Position.x - centerX);
		float depth = fabsf(m_VertexList[i].Position.z - centerZ);

		if (width > maxWidth) { maxWidth = width; }
		if (depth > maxDepth) { maxDepth = depth; }
		if (width < minWidth) { minWidth = width; }
		if (depth < minDepth) { minDepth = depth; }
	}

	// 최소와 최대 깊이와 너비 사이의 절대 최대 값을 찾습니다.
	float maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	// 메쉬의 최대 직경을 계산합니다.
	width = max(maxX, maxZ) * 2.0f;

}

void QuadtreeClass::CreateQuadtree(ID3D11Device* device, NodeType* Node, float PositionX, float PositionZ, float width)
{
	// 노드의 위치와 크기를 저장한다.
	Node->positionX = PositionX;
	Node->positionZ = PositionZ;
	Node->width = width;

	// 노드의 삼각형 수를 0으로 초기화합니다.
	Node->triangleCount = 0;

	//정점 및 인덱스 버퍼를 null로 초기화합니다.
	Node->vertexBuffer = 0;
	Node->indexBuffer = 0;

	// 이 노드의 자식 노드를 null로 초기화합니다.
	Node->Child[0] = 0;
	Node->Child[1] = 0;
	Node->Child[2] = 0;
	Node->Child[3] = 0;

	// 이 노드 안에 있는 삼각형 수를 센다.
	int numTriangles = CountTriangles(PositionX, PositionZ, width);

	// 사례 1: 이 노드에 삼각형이 없으면 비어있는 상태로 돌아가서 처리할 필요가 없습니다.
	if (numTriangles == 0)
	{
		return;
	}

	// 사례 2: 이 노드에 너무 많은 삼각형이 있는 경우 4 개의 동일한 크기의 더 작은 트리 노드로 분할합니다.
	if (numTriangles > MAX_TRIANGLES)
	{
		for (int i = 0; i < 4; i++)
		{
			// 새로운 자식 노드에 대한 위치 오프셋을 계산합니다.
			float offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			// 새 노드에 삼각형이 있는지 확인합니다.
			int count = CountTriangles((PositionX + offsetX), (PositionZ + offsetZ), (width / 2.0f));
			if (count > 0)
			{
				// 이 새 노드가있는 삼각형이있는 경우 자식 노드를 만듭니다.
				Node->Child[i] = new NodeType;

				// 이제이 새 자식 노드에서 시작하는 트리를 확장합니다.
				CreateQuadtree(device, Node->Child[i], (PositionX + offsetX), (PositionZ + offsetZ), (width / 2.0f));
			}
		}
		return;
	}

	// 사례 3: 이 노드가 비어 있지않고 그 노드의 삼각형 수가 최대 값보다 작으면 
	// 이 노드는 트리의 맨 아래에 있으므로 저장할 삼각형 목록을 만듭니다.
	Node->triangleCount = numTriangles;

	// 정점의 수를 계산합니다.
	int vertexCount = numTriangles * 3;

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[vertexCount];

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[vertexCount];

	// 이 새로운 정점 및 인덱스 배열의 인덱스를 초기화합니다.
	int index = 0;
	int vertexIndex = 0;

	// 정점 목록의 모든 삼각형을 살펴 봅니다.
	for (int i = 0; i < m_triangleCount; i++)
	{
		// 삼각형이이 노드 안에 있으면 꼭지점 배열에 추가합니다.
		if (IsTriangleContained(i, PositionX, PositionZ, width) == true)
		{
			// 지형 버텍스 목록에 인덱스를 계산합니다.
			vertexIndex = i * 3;

			// 정점 목록에서 이 삼각형의 세 꼭지점을 가져옵니다.
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

	// 정점 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 마침내 정점 버퍼를 만듭니다.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &Node->vertexBuffer);

	// 인덱스 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다.
	device->CreateBuffer(&indexBufferDesc, &indexData, &Node->indexBuffer);

	// 이제 노드의 버퍼에 데이터가 저장되므로 꼭지점과 인덱스 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	
}

int QuadtreeClass::CountTriangles(float positionX, float positionZ, float width)
{
	// 카운트를 0으로 초기화한다.
	int count = 0;

	// 전체 메쉬의 모든 삼각형을 살펴보고 어떤 노드가 이 노드 안에 있어야 하는지 확인합니다.
	for (int i = 0; i < m_triangleCount; i++)
	{
		// 삼각형이 노드 안에 있으면 1씩 증가시킵니다.
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
		{
			count++;
		}
	}

	return count;
}

bool QuadtreeClass::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
	// 이 노드의 반경을 계산합니다.
	float radius = width / 2.0f;

	// 인덱스를 정점 목록으로 가져옵니다.
	int vertexIndex = index * 3;

	// 정점 목록에서 이 삼각형의 세 꼭지점을 가져옵니다.
	float x1 = m_VertexList[vertexIndex].Position.x;
	float z1 = m_VertexList[vertexIndex].Position.z;
	vertexIndex++;

	float x2 = m_VertexList[vertexIndex].Position.x;
	float z2 = m_VertexList[vertexIndex].Position.z;
	vertexIndex++;

	float x3 = m_VertexList[vertexIndex].Position.x;
	float z3 = m_VertexList[vertexIndex].Position.z;

	// 삼각형의 x 좌표의 최소값이 노드 안에 있는지 확인하십시오.
	float minimumX = min(x1, min(x2, x3));
	if (minimumX > (positionX + radius))
	{
		return false;
	}

	// 삼각형의 x 좌표의 최대 값이 노드 안에 있는지 확인하십시오.
	float maximumX = max(x1, max(x2, x3));
	if (maximumX < (positionX - radius))
	{
		return false;
	}

	// 삼각형의 z 좌표의 최소값이 노드 안에 있는지 확인하십시오.
	float minimumZ = min(z1, min(z2, z3));
	if (minimumZ > (positionZ + radius))
	{
		return false;
	}

	// 삼각형의 z 좌표의 최대 값이 노드 안에 있는지 확인하십시오.
	float maximumZ = max(z1, max(z2, z3));
	if (maximumZ < (positionZ - radius))
	{
		return false;
	}

	return true;
}

void QuadtreeClass::RenderNode(NodeType* node, FrustumClass* frustum, ID3D11DeviceContext* devcon, TerrainShaderClass* shader)
{
	// 현재 노드가 보이는지 Frustum을 통해 판단
	if (!frustum->CheckCube(node->positionX, 0, node->positionZ, node->width))
		return;

	// 자식 노드가 있는 경우 자식 노드에 대해 RenderNode
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (node->Child[i] != 0)
		{
			RenderNode(node->Child[i], frustum, devcon, shader);
			count++;
		}
	}

	// 자식 노드가 있는 경우 부모는 Render 할 필요가 없다.
	if (count != 0)
	{
		return;
	}

	// 자식 노드가 없는 경우 Render

	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	devcon->IASetVertexBuffers(0, 1, &node->vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	devcon->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링 되어야 하는 프리미티브 유형을 설정합니다. 이 경우에는 삼각형입니다.
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 이 노드에서 인덱스의 수를 결정합니다.
	int indexCount = node->triangleCount * 3;

	// 지형 셰이더를 호출하여 이 노드의 다각형을 렌더링합니다.
	shader->RenderShader(devcon, indexCount);

	// 이 프레임 동안 렌더링 된 폴리곤의 수를 늘립니다.
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
