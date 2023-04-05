#include "TerrainClass.h"
#include "TextureClass.h"
TerrainClass::TerrainClass()
{
}

TerrainClass::TerrainClass(const TerrainClass&)
{
}

TerrainClass::~TerrainClass()
{
}

bool TerrainClass::Initialize(ID3D11Device* device, const char* HeightMapFileName, const WCHAR* TextureFilename)
{
    if (!LoadHeightMap(HeightMapFileName))
        return false;

	NormalizeHeightMap();

	if (!CalculateNormals())
		return false;

	CalculateTextureCoord();

	if (!LoadTexture(device, TextureFilename))
		return false;

    
    if (!InitializeBuffers(device))
        return false;

    return true;
}

void TerrainClass::shutdown()
{
	ShutdownHeightMap();
	ShutdownBuffers();
	ReleaseTexture();
}

void TerrainClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼, 인덱스 버퍼를 파이프라인에 설정한다.
	RenderBuffers(deviceContext);
}

int TerrainClass::GetVertexCount()
{
	return m_VertexCount;
}

int TerrainClass::GetIndexCount()
{
    return m_IndexCount;
}

void TerrainClass::CopyVertexArray(void* vertexList)
{
	memcpy(vertexList, m_Vertices, sizeof(VertexType) * m_VertexCount);
}

ID3D11ShaderResourceView* TerrainClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool TerrainClass::LoadHeightMap(const char* HeightMapFileName)
{
    FILE* BitMapFileptr = nullptr;

    // 바이너리 읽기 모드로 높이맵 파일을 연다.
    if (fopen_s(&BitMapFileptr, HeightMapFileName, "rb") != 0)
    {
        return false;
    }

    // 비트맵 파일 헤더를 읽는다.
    BITMAPFILEHEADER bitmapFileHeader;
    if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, BitMapFileptr) != 1)
        return false;

    // 비트맵 정보 헤더를 읽는다.
    BITMAPINFOHEADER bitmapInfoHeader;
    if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, BitMapFileptr) != 1)
        return false;

    // 지형의 크기 저장
    m_terrainHeight = bitmapInfoHeader.biHeight;
    m_terrainWidth = bitmapInfoHeader.biWidth;

    // 비트맵 이미지 데이터의 크기 계산
    int imageSize = m_terrainHeight * m_terrainWidth * 3;

    // 비트맵 이미지 데이터에 메모리 할당
    unsigned char* bitMapImage = new unsigned char[imageSize];
    if (!bitMapImage)
        return false;

    // 비트맵 데이터의 시작 부분으로 이동
    fseek(BitMapFileptr, bitmapFileHeader.bfOffBits, SEEK_SET);

    // 비트맵 이미지 데이터 읽기
    if (fread(bitMapImage, 1, imageSize, BitMapFileptr) != imageSize)
    {
        return false;
    }

    // 열었던 비트맵 파일 닫기
    if (fclose(BitMapFileptr) != 0)
        return false;

    // 높이맵 데이터 저장할 구조체 생성
    m_heightMap = new HeightMapType[m_terrainHeight * m_terrainWidth];
    if (!m_heightMap)
        return false;

    // 이미지 데이터 버퍼의 위치를 ​​초기화합니다.
    int k = 0;

    // 이미지 데이터를 높이 맵으로 읽어들입니다.
	// 높이맵은 RGB값이 모두 같으므로(검정~흰색) RGB중 하나만 저장한다.
	// 따라서 읽어들인 비트맵 이미지는 픽셀순으로 R, G, B를 모두 저장하므로, 이에 접근할 때 인덱스를 3씩 더한다.
    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            unsigned char height = bitMapImage[k];

            int index = (m_terrainHeight * j) + i;

            m_heightMap[index].x = (float)i;
            m_heightMap[index].y = (float)height;
            m_heightMap[index].z = (float)j;

            k += 3;
        }
    }

    // 비트맵 이미지 데이터를 해제합니다.
    delete[] bitMapImage;
    bitMapImage = 0;

    return true;
}

void TerrainClass::NormalizeHeightMap()
{
    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
        }
    }
}

bool TerrainClass::CalculateNormals()
{
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index = 0;
	int count = 0;
	float vertex1[3] = { 0.f, 0.f, 0.f };
	float vertex2[3] = { 0.f, 0.f, 0.f };
	float vertex3[3] = { 0.f, 0.f, 0.f };
	float vector1[3] = { 0.f, 0.f, 0.f };
	float vector2[3] = { 0.f, 0.f, 0.f };
	float sum[3] = { 0.f, 0.f, 0.f };
	float length = 0.0f;


	// 정규화되지 않은 법선 벡터를 저장할 임시 배열을 만듭니다.
	VectorType* normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// 메쉬의 모든면을 살펴보고 법선을 계산합니다.
	for (int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			// 표면에서 세 개의 꼭지점을 가져옵니다.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// 표면의 두 벡터를 계산합니다.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			// 이 두 법선에 대한 정규화되지 않은 값을 얻기 위해 두 벡터의 외적을 계산합니다.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// 이제 모든 정점을 살펴보고 각면의 평균을 취합니다. 	
	// 정점이 닿아 그 정점에 대한 평균 평균값을 얻는다.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			// 합계를 초기화합니다.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// 카운트를 초기화합니다.
			count = 0;

			// 왼쪽 아래면.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 오른쪽 아래 면.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 왼쪽 위 면.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 오른쪽 위 면.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 이 정점에 닿는면의 평균을 취합니다.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// 이 법선의 길이를 계산합니다.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// 높이 맵 배열의 정점 위치에 대한 인덱스를 가져옵니다.
			index = (j * m_terrainHeight) + i;

			// 이 정점의 최종 공유 법선을 표준화하여 높이 맵 배열에 저장합니다.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// 임시 법선을 해제합니다.
	delete[] normals;
	normals = 0;

	return true;
}


void TerrainClass::CalculateTextureCoord()
{
	// 텍스처 좌표를 얼마나 많이 증가 시킬지 계산합니다.
	float incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;

	// 텍스처를 반복 할 횟수를 계산합니다.
	int incrementCount = m_terrainWidth / TEXTURE_REPEAT;

	// tu 및 tv 좌표 값을 초기화합니다.
	float tuCoordinate = 0.0f;
	float tvCoordinate = 1.0f;

	//  tu 및 tv 좌표 인덱스를 초기화합니다.
	int tuCount = 0;
	int tvCount = 0;

	// 전체 높이 맵을 반복하고 각 꼭지점의 tu 및 tv 텍스처 좌표를 계산합니다.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			// 높이 맵에 텍스처 좌표를 저장한다.
			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;

			// tu 텍스처 좌표를 증가 값만큼 증가시키고 인덱스를 1 씩 증가시킨다.
			tuCoordinate += incrementValue;
			tuCount++;

			// 텍스처의 오른쪽 끝에 있는지 확인하고, 그렇다면 처음부터 다시 시작하십시오.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// tv 텍스처 좌표를 증가 값만큼 증가시키고 인덱스를 1 씩 증가시킵니다.
		tvCoordinate -= incrementValue;
		tvCount++;

		// 텍스처의 상단에 있는지 확인하고, 그렇다면 하단에서 다시 시작합니다.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}
}

void TerrainClass::ShutdownHeightMap()
{
    if (m_heightMap)
    {
        delete[] m_heightMap;
        m_heightMap = 0;
    }
}

bool TerrainClass::LoadTexture(ID3D11Device* device, const WCHAR* TextureFilename)
{
	m_Texture = new TextureClass;
	if (!m_Texture)
		return false;

	if (!m_Texture->Initialize(device, TextureFilename))
		return false;

	return true;
}

void TerrainClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}


bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	// 지형 메쉬의 정점 수를 계산합니다.
	m_VertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 12;

	// 인덱스 수를 꼭지점 수와 같게 설정합니다.
	m_IndexCount = m_VertexCount;

	// 정점 배열을 만듭니다.
	m_Vertices = new VertexType[m_VertexCount];
	if (!m_Vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[m_IndexCount];
	if (!indices)
	{
		return false;
	}

	// 정점 배열에 대한 인덱스를 초기화합니다.
	int index = 0;

	float tu = 0.0f;
	float tv = 0.0f;

	// 지형 데이터로 정점 및 인덱스 배열을 로드합니다.
	for (int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i < (m_terrainWidth - 1); i++)
		{
			int index1 = (m_terrainHeight * j) + i;          // 왼쪽 아래.
			int index2 = (m_terrainHeight * j) + (i + 1);      // 오른쪽 아래.
			int index3 = (m_terrainHeight * (j + 1)) + i;      // 왼쪽 위.
			int index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // 오른쪽 위.

			// 왼쪽 위.
			tv = m_heightMap[index3].tv;

			// 상단 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tv == 1.0f) { tv = 0.0f; }

			m_Vertices[index].position = XMFLOAT3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			m_Vertices[index].texcoords = XMFLOAT2(m_heightMap[index3].tu, tv);
			m_Vertices[index].normal = XMFLOAT3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			indices[index] = index;
			index++;

			// 오른쪽 위.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// 위쪽과 오른쪽 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			m_Vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_Vertices[index].texcoords = XMFLOAT2(tu, tv);
			m_Vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// 왼쪽 아래.
			m_Vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_Vertices[index].texcoords = XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			m_Vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// 왼쪽 아래.
			m_Vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_Vertices[index].texcoords = XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			m_Vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// 오른쪽 위.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// 위쪽과 오른쪽 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			m_Vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_Vertices[index].texcoords = XMFLOAT2(tu, tv);
			m_Vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// 오른쪽 아래.
			tu = m_heightMap[index2].tu;

			// 오른쪽 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tu == 0.0f) { tu = 1.0f; }

			m_Vertices[index].position = XMFLOAT3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			m_Vertices[index].texcoords = XMFLOAT2(tu, m_heightMap[index2].tv);
			m_Vertices[index].normal = XMFLOAT3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			indices[index] = index;
			index++;
		}
	}

	// 정적 정점 버퍼의 구조체를 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_Vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
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
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 이제 버퍼가 생성되고 로드된 배열을 해제하십시오.
	delete[] indices;
	indices = 0;

	return true;
}

void TerrainClass::ShutdownBuffers()
{
	// 인덱스 버퍼를 해제합니다.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 버텍스 버퍼를 해제합니다.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 버텍스 버퍼에서 렌더링되어야하는 프리미티브의 타입을 설정한다.이 경우 라인리스트이다.
	deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
