#include "Terrain.h"
#include "..\stdafx.h"
#include "TerrainGraphicC.h"
#include "StaticPhysicsC.h"
#include "InputClass.h"
#include "Ray.h"
#include "D3DClass.h"

Terrain::Terrain()
{
	m_Brush.type = 0;
	m_Brush.color = XMFLOAT3(0.0f, 0.5f, 0.0f);
	m_Brush.location = XMFLOAT3(0.f, 0.f, 0.f);
	m_Brush.range = 10.0f;
}

Terrain::~Terrain()
{
}

bool Terrain::Initialize(ID3D11Device* device, const char* HeightMapFileName, const WCHAR* TextureFilename)
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

	if (!InitializeComponent(device))
		return false;

	if (!InitializeComputeShader(device))
		return false;

	ShutdownHeightMap();

	return true;
}

void Terrain::shutdown()
{
	ShutdownBuffers();
	ReleaseTexture();
}

bool Terrain::UpdateInput(InputClass* input, float fDeltaTime)
{
	if (!m_InputClass)
		m_InputClass = input;

	if (m_IsDraw == true)
	{
		
		if(input->IsMouseLeftClicked())
			UpdateTerrainHeight(m_Brush.location, fDeltaTime);
		/*
		if (input->MousePress(0))
		{
			UpdateTerrainHeight(m_Brush.location, fDeltaTime);
		}
		if (input->MouseUp(0))
		{

		}
		*/
	}

	return true;
}

bool Terrain::Render(ID3D11DeviceContext* devcon)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	if (FAILED(devcon->Map(m_BrushBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource)))
		return false;

	Brush* dataPtr = (Brush*)subResource.pData;
	dataPtr->type = m_Brush.type;
	dataPtr->location = m_Brush.location;
	dataPtr->range = m_Brush.range;
	dataPtr->color = m_Brush.color;

	devcon->Unmap(m_BrushBuffer, 0);

	unsigned int bufferNumber = 1;
	devcon->PSSetConstantBuffers(bufferNumber, 1, &m_BrushBuffer);

	if (!m_GraphicC->Update(devcon, this))
		return false;

	return true;
}

bool Terrain::PostRender(ID3D11DeviceContext* devcon)
{
	XMFLOAT3 position;
	ComputePicking(devcon, &position);

	m_Brush.location = position;

	m_Brushlocation[0] = position.x;
	m_Brushlocation[1] = position.y;
	m_Brushlocation[2] = position.z;
	//
	// ImGui::Spacing();
	ImGui::Text("[Terrain Editor]");
	ImGui::Checkbox("IsDraw", &m_IsDraw);
	ImGui::InputFloat3("Brush location", m_Brushlocation);
	ImGui::InputFloat2("Mouse location", m_MousePosition);
	ImGui::InputFloat3("Mouse World location", m_MouseWorldPosition);
	//ImGui::SliderFloat3("Brush location", m_Brushlocation, 0, )
	ImGui::SliderInt("Type",
		&m_Brush.type, 0, 1);
	ImGui::SliderFloat("Range",
		&m_Brush.range, 1.0f, 50.0f);
	ImGui::ColorEdit3("Color",
		(float*)&m_Brush.color);
	ImGui::Checkbox("Raise", &m_IsRaise);
	//



	return true;
}

ID3D11ShaderResourceView* Terrain::GetTexture()
{
	return m_Texture;
}


bool Terrain::LoadHeightMap(const char* HeightMapFileName)
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

			// int index = (m_terrainHeight * j) + i;
			int index = (m_terrainWidth * j) + i;

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

void Terrain::NormalizeHeightMap()
{
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
		}
	}
}

bool Terrain::CalculateNormals()
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


void Terrain::CalculateTextureCoord()
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

void Terrain::ShutdownHeightMap()
{

	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}
}

bool Terrain::LoadTexture(ID3D11Device* device, const WCHAR* TextureFilename)
{
	if (FAILED(CreateDDSTextureFromFile(device, TextureFilename, nullptr, &m_Texture)))
	{
		return false;
	}

	return true;
}

void Terrain::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Release();
		m_Texture = 0;
	}
}


bool Terrain::InitializeBuffers(ID3D11Device* device)
{
	// 지형 메쉬의 정점 수를 계산합니다.
	m_VertexCount = (m_terrainWidth) * (m_terrainHeight);

	// 인덱스 수를 꼭지점 수와 같게 설정합니다.
	m_IndexCount = m_VertexCount * 6;

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
	
	for (int j = 0; j < (m_terrainHeight); j++)
	{
		for (int i = 0; i < (m_terrainWidth); i++)
		{
			index = m_terrainWidth * j + i;
			
			m_Vertices[index].position = XMFLOAT3(m_heightMap[index].x, m_heightMap[index].y, m_heightMap[index].z);
			m_Vertices[index].texcoords = XMFLOAT2(m_heightMap[index].tu, m_heightMap[index].tv);
			m_Vertices[index].normal = XMFLOAT3(m_heightMap[index].nx, m_heightMap[index].ny, m_heightMap[index].nz);
		}
	}

	
	int count = 0;
	for (int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i < (m_terrainWidth - 1); i++)
		{
			indices[count++] = (m_terrainWidth) * j + i;	//0
			indices[count++] = ((m_terrainWidth) * (j + 1) + i);		//1
			indices[count++] = (m_terrainWidth) * (j + 1) + i + 1;	//2

			indices[count++] = ((m_terrainWidth) * j + i);			//0
			indices[count++] = ((m_terrainWidth) * (j + 1) + i + 1);	//2
			indices[count++] = ((m_terrainWidth) * j + i + 1);		//3
		}
	}

	m_IndexCount = count;

	m_input = new InputDesc[count / 3];


	for (int i = 0; i < count / 3; i++)
	{
		if (i == 30000)
		{
			int t = 1;
		}
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		m_input[i].v0 = XMFLOAT3(m_Vertices[index0].position.x, m_Vertices[index0].position.y, m_Vertices[index0].position.z);
		m_input[i].v1 = XMFLOAT3(m_Vertices[index1].position.x, m_Vertices[index1].position.y, m_Vertices[index1].position.z);
		m_input[i].v2 = XMFLOAT3(m_Vertices[index2].position.x, m_Vertices[index2].position.y, m_Vertices[index2].position.z);

		m_input[i].index = i;
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
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer)))
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
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer)))
	{
		return false;
	}

	// 이제 버퍼가 생성되고 로드된 배열을 해제하십시오.
	delete[] indices;
	indices = 0;
	
	
	return true;
}

bool Terrain::InitializeComponent(ID3D11Device* dev)
{
	m_GraphicC = new TerrainGraphicC();
	if (!m_GraphicC)
		return false;
	if (!m_GraphicC->Initialize(dev))
		return false;


	m_PhysicsC = new StaticPhysicsC();
	if (!m_PhysicsC)
		return false;

	PxHeightFieldSample* samples = new PxHeightFieldSample[m_terrainHeight * m_terrainWidth];

	for (PxU32 i = 0; i < m_terrainHeight * m_terrainWidth; ++i)
	{
		samples[i].height = static_cast<PxI16>(m_heightMap[i].y); // 높이 값 설정
		samples[i].materialIndex0 = 0; // 첫 번째 머티리얼 인덱스 설정
		samples[i].materialIndex1 = 1; // 두 번째 머티리얼 인덱스 설정 (optional)
	}
	
	PxVec3 trans = PxVec3(m_Position.x, m_Position.y, m_Position.z);

	PxTransform transform = PxTransform(trans);
	PxHeightFieldDesc heightfieldDesc;
	heightfieldDesc.format = PxHeightFieldFormat::eS16_TM;
	heightfieldDesc.nbColumns = m_terrainHeight;
	heightfieldDesc.nbRows = m_terrainWidth;
	heightfieldDesc.samples.data = samples;
	heightfieldDesc.samples.stride = sizeof(float);

	PxHeightField* heightField = PhysicsComponent::GetCooking()->createHeightField(heightfieldDesc,
		PhysicsComponent::GetPhysics()->getPhysicsInsertionCallback());


	m_HeightFieldGeometry = PxHeightFieldGeometry(heightField, PxMeshGeometryFlags(), m_Scale.y, m_Scale.x, m_Scale.z);
	PxMaterial* material = PhysicsComponent::GetPhysics()->createMaterial(0.5f, 0.5f, 0.5);

	if (!m_PhysicsC->Initialize(transform, m_HeightFieldGeometry, material))
		return false;

	delete[] samples;

	return true;
}

void Terrain::ShutdownBuffers()
{
	// 인덱스 버퍼를 해제합니다.
	if (m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = 0;
	}

	// 버텍스 버퍼를 해제합니다.
	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = 0;
	}
}


void Terrain::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
	// 메쉬의 중심 위치를 0으로 초기화합니다.
	centerX = 0.0f;
	centerZ = 0.0f;

	// 메쉬의 모든 정점을 합친다.
	for (int i = 0; i < vertexCount; i++)
	{
		centerX += m_Vertices[i].position.x;
		centerZ += m_Vertices[i].position.z;
	}

	// 그리고 메쉬의 중간 점을 찾기 위해 정점의 수로 나눕니다.
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// 메쉬의 최대 및 최소 크기를 초기화합니다.
	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	float minWidth = fabsf(m_Vertices[0].position.x - centerX);
	float minDepth = fabsf(m_Vertices[0].position.z - centerZ);

	// 모든 정점을 살펴보고 메쉬의 최대 너비와 최소 너비와 깊이를 찾습니다.
	for (int i = 0; i < vertexCount; i++)
	{
		float width = fabsf(m_Vertices[i].position.x - centerX);
		float depth = fabsf(m_Vertices[i].position.z - centerZ);

		if (width > maxWidth) { maxWidth = width; }
		if (depth > maxDepth) { maxDepth = depth; }
		if (width < minWidth) { minWidth = width; }
		if (depth < minDepth) { minDepth = depth; }
	}

	// 최소와 최대 깊이와 너비 사이의 절대 최대 값을 찾습니다.
	float maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	// 메쉬의 최대 직경을 계산합니다.
	meshWidth = max(maxX, maxZ) * 2.0f;
}


void Terrain::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device)
{
	// 노드의 위치와 크기를 저장한다.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	// 노드의 삼각형 수를 0으로 초기화합니다.
	node->triangleCount = 0;

	//정점 및 인덱스 버퍼를 null로 초기화합니다.
	node->vertexBuffer = 0;
	node->indexBuffer = 0;

	// Initialize the vertex array to null.
	node->vertexArray = 0;

	// 이 노드의 자식 노드를 null로 초기화합니다.
	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	// 이 노드 안에 있는 삼각형 수를 센다.
	int numTriangles = CountTriangles(positionX, positionZ, width);

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
			int count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if (count > 0)
			{
				// 이 새 노드가있는 삼각형이있는 경우 자식 노드를 만듭니다.
				node->nodes[i] = new NodeType;

				// 이제이 새 자식 노드에서 시작하는 트리를 확장합니다.
				CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), device);
			}
		}
		return;
	}

	// 사례 3: 이 노드가 비어 있지않고 그 노드의 삼각형 수가 최대 값보다 작으면 
	// 이 노드는 트리의 맨 아래에 있으므로 저장할 삼각형 목록을 만듭니다.
	node->triangleCount = numTriangles;

	// 정점의 수를 계산합니다.
	int vertexCount = numTriangles * 3;

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[vertexCount];

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[vertexCount];

	// 정점 배열을 만듭니다.
	node->vertexArray = new VectorType[vertexCount];

	// 이 새로운 정점 및 인덱스 배열의 인덱스를 초기화합니다.
	int index = 0;

	// 정점 목록의 모든 삼각형을 살펴 봅니다.
	int vertexIndex = 0;
	for (int i = 0; i < m_triangleCount; i++)
	{
		// 삼각형이이 노드 안에 있으면 꼭지점 배열에 추가합니다.
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
		{
			// 지형 버텍스 목록에 인덱스를 계산합니다.
			vertexIndex = i * 3;

			// 정점 목록에서 이 삼각형의 세 꼭지점을 가져옵니다.
			vertices[index].position = m_Vertices[vertexIndex].position;
			vertices[index].texcoords = m_Vertices[vertexIndex].texcoords;
			vertices[index].normal = m_Vertices[vertexIndex].normal;
			indices[index] = index;

			// 또한 정점 위치 정보를 노드 정점 배열에 저장합니다.
			node->vertexArray[index].x = m_Vertices[vertexIndex].position.x;
			node->vertexArray[index].y = m_Vertices[vertexIndex].position.y;
			node->vertexArray[index].z = m_Vertices[vertexIndex].position.z;

			// 인덱스 값을 증가합니다.
			index++;

			vertexIndex++;

			// 다음 요점에 대해서도 똑같이하십시오.
			vertices[index].position = m_Vertices[vertexIndex].position;
			vertices[index].texcoords = m_Vertices[vertexIndex].texcoords;
			vertices[index].normal = m_Vertices[vertexIndex].normal;
			indices[index] = index;
			node->vertexArray[index].x = m_Vertices[vertexIndex].position.x;
			node->vertexArray[index].y = m_Vertices[vertexIndex].position.y;
			node->vertexArray[index].z = m_Vertices[vertexIndex].position.z;
			index++;

			vertexIndex++;

			// 다음 요점에 대해서도 똑같이하십시오.
			vertices[index].position = m_Vertices[vertexIndex].position;
			vertices[index].texcoords = m_Vertices[vertexIndex].texcoords;
			vertices[index].normal = m_Vertices[vertexIndex].normal;
			indices[index] = index;
			node->vertexArray[index].x = m_Vertices[vertexIndex].position.x;
			node->vertexArray[index].y = m_Vertices[vertexIndex].position.y;
			node->vertexArray[index].z = m_Vertices[vertexIndex].position.z;
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
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &node->vertexBuffer);

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
	device->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);

	// 이제 노드의 버퍼에 데이터가 저장되므로 꼭지점과 인덱스 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}


int Terrain::CountTriangles(float positionX, float positionZ, float width)
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


bool Terrain::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
	// 이 노드의 반경을 계산합니다.
	float radius = width / 2.0f;

	// 인덱스를 정점 목록으로 가져옵니다.
	int vertexIndex = index * 3;

	// 정점 목록에서 이 삼각형의 세 꼭지점을 가져옵니다.
	float x1 = m_Vertices[vertexIndex].position.x;
	float z1 = m_Vertices[vertexIndex].position.z;
	vertexIndex++;

	float x2 = m_Vertices[vertexIndex].position.x;
	float z2 = m_Vertices[vertexIndex].position.z;
	vertexIndex++;

	float x3 = m_Vertices[vertexIndex].position.x;
	float z3 = m_Vertices[vertexIndex].position.z;

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


void Terrain::ReleaseNode(NodeType* node)
{
	// 재귀적으로 트리 아래로 내려와 맨 아래 노드를 먼저 놓습니다.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			ReleaseNode(node->nodes[i]);
		}
	}

	// 이 노드의 버텍스 버퍼를 해제한다.
	if (node->vertexBuffer)
	{
		node->vertexBuffer->Release();
		node->vertexBuffer = 0;
	}

	// 이 노드의 인덱스 버퍼를 해제합니다.
	if (node->indexBuffer)
	{
		node->indexBuffer->Release();
		node->indexBuffer = 0;
	}

	// 이 노드의 정점 배열을 해제합니다.
	if (node->vertexArray)
	{
		delete[] node->vertexArray;
		node->vertexArray = 0;
	}

	// 4개의 자식 노드를 해제합니다.
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}
}

bool Terrain::GetHeightAtPosition(float positionX, float positionZ, float& height)
{
	float meshMinX = m_parentNode->positionX - (m_parentNode->width / 2.0f);
	float meshMaxX = m_parentNode->positionX + (m_parentNode->width / 2.0f);

	float meshMinZ = m_parentNode->positionZ - (m_parentNode->width / 2.0f);
	float meshMaxZ = m_parentNode->positionZ + (m_parentNode->width / 2.0f);

	// 좌표가 실제로 다각형 위에 있는지 확인하십시오.
	if ((positionX < meshMinX) || (positionX > meshMaxX) || (positionZ < meshMinZ) || (positionZ > meshMaxZ))
	{
		return false;
	}

	// 이 위치에 대한 다각형을 포함하는 노드를 찾습니다.
	FindNode(m_parentNode, positionX, positionZ, height);

	return true;
}


void Terrain::FindNode(NodeType* node, float x, float z, float& height)
{
	// 이 노드의 크기를 계산합니다.
	float xMin = node->positionX - (node->width / 2.0f);
	float xMax = node->positionX + (node->width / 2.0f);

	float zMin = node->positionZ - (node->width / 2.0f);
	float zMax = node->positionZ + (node->width / 2.0f);

	// x 및 z 좌표가이 노드에 있는지 확인합니다. 그렇지 않으면 트리의이 부분을 탐색하지 않습니다.
	if ((x < xMin) || (x > xMax) || (z < zMin) || (z > zMax))
	{
		return;
	}

	// 좌표가 이 노드에 있으면 자식 노드가 있는지 먼저 확인합니다.
	int count = 0;

	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			count++;
			FindNode(node->nodes[i], x, z, height);
		}
	}

	// 자식 노드가 있는 경우 폴리곤이 자식중 하나에 있으므로 노드가 반환됩니다.
	if (count > 0)
	{
		return;
	}

	float vertex1[3] = { 0.0f, 0.0f, 0.0f };
	float vertex2[3] = { 0.0f, 0.0f, 0.0f };
	float vertex3[3] = { 0.0f, 0.0f, 0.0f };

	// 자식이 없으면 다각형이이 노드에 있어야합니다. 이 노드의 모든 다각형을 확인하여 찾습니다.
	// 우리가 찾고있는 폴리곤의 높이.
	for (int i = 0; i < node->triangleCount; i++)
	{
		int index = i * 3;
		vertex1[0] = node->vertexArray[index].x;
		vertex1[1] = node->vertexArray[index].y;
		vertex1[2] = node->vertexArray[index].z;

		index++;
		vertex2[0] = node->vertexArray[index].x;
		vertex2[1] = node->vertexArray[index].y;
		vertex2[2] = node->vertexArray[index].z;

		index++;
		vertex3[0] = node->vertexArray[index].x;
		vertex3[1] = node->vertexArray[index].y;
		vertex3[2] = node->vertexArray[index].z;

		// 이것이 우리가 찾고있는 폴리곤인지 확인합니다.
		// 삼각형 인 경우 함수를 종료하고 높이가 호출 함수에 반환됩니다.
		if (CheckHeightOfTriangle(x, z, height, vertex1, vertex2, vertex3))
		{
			return;
		}
	}
}


bool Terrain::CheckHeightOfTriangle(float x, float z, float& height, float v0[3], float v1[3], float v2[3])
{
	float startVector[3] = { 0.0f, 0.0f, 0.0f };
	float directionVector[3] = { 0.0f, 0.0f, 0.0f };
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float normal[3] = { 0.0f, 0.0f, 0.0f };
	float Q[3] = { 0.0f, 0.0f, 0.0f };
	float e1[3] = { 0.0f, 0.0f, 0.0f };
	float e2[3] = { 0.0f, 0.0f, 0.0f };
	float e3[3] = { 0.0f, 0.0f, 0.0f };
	float edgeNormal[3] = { 0.0f, 0.0f, 0.0f };
	float temp[3] = { 0.0f, 0.0f, 0.0f };

	// 전송중인 광선의 시작 위치.
	startVector[0] = x;
	startVector[1] = 0.0f;
	startVector[2] = z;

	// 광선이 투영되는 방향입니다.
	directionVector[0] = 0.0f;
	directionVector[1] = -1.0f;
	directionVector[2] = 0.0f;

	// 주어진 세 점으로부터 두 모서리를 계산합니다.
	edge1[0] = v1[0] - v0[0];
	edge1[1] = v1[1] - v0[1];
	edge1[2] = v1[2] - v0[2];

	edge2[0] = v2[0] - v0[0];
	edge2[1] = v2[1] - v0[1];
	edge2[2] = v2[2] - v0[2];

	// 두 모서리에서 삼각형의 법선을 계산합니다.
	normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
	normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
	normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

	float magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
	normal[0] = normal[0] / magnitude;
	normal[1] = normal[1] / magnitude;
	normal[2] = normal[2] / magnitude;

	// 원점에서 평면까지의 거리를 구합니다.
	float D = ((-normal[0] * v0[0]) + (-normal[1] * v0[1]) + (-normal[2] * v0[2]));

	// 방정식의 분모를 구하십시오.
	float denominator = ((normal[0] * directionVector[0]) + (normal[1] * directionVector[1]) + (normal[2] * directionVector[2]));

	// 결과가 0에 너무 가까워지지 않도록하여 0으로 나누는 것을 방지하십시오.
	if (fabs(denominator) < 0.0001f)
	{
		return false;
	}

	// 방정식의 분자를 구합니다.
	float numerator = -1.0f * (((normal[0] * startVector[0]) + (normal[1] * startVector[1]) + (normal[2] * startVector[2])) + D);

	// 삼각형과 교차하는 위치를 계산합니다.
	float t = numerator / denominator;

	// 교차 벡터를 찾습니다.
	Q[0] = startVector[0] + (directionVector[0] * t);
	Q[1] = startVector[1] + (directionVector[1] * t);
	Q[2] = startVector[2] + (directionVector[2] * t);

	// 삼각형의 세 모서리를 찾습니다.
	e1[0] = v1[0] - v0[0];
	e1[1] = v1[1] - v0[1];
	e1[2] = v1[2] - v0[2];

	e2[0] = v2[0] - v1[0];
	e2[1] = v2[1] - v1[1];
	e2[2] = v2[2] - v1[2];

	e3[0] = v0[0] - v2[0];
	e3[1] = v0[1] - v2[1];
	e3[2] = v0[2] - v2[2];

	// 첫 번째 가장자리의 법선을 계산합니다.
	edgeNormal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
	edgeNormal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
	edgeNormal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

	// 행렬이 내부, 외부 또는 직접 가장자리에 있는지 결정하기 위해 행렬식을 계산합니다.
	temp[0] = Q[0] - v0[0];
	temp[1] = Q[1] - v0[1];
	temp[2] = Q[2] - v0[2];

	float determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// 외부에 있는지 확인하십시오.
	if (determinant > 0.001f)
	{
		return false;
	}

	// 두 번째 가장자리의 법선을 계산합니다.
	edgeNormal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
	edgeNormal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
	edgeNormal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

	// 행렬이 내부, 외부 또는 직접 가장자리에 있는지 결정하기 위해 행렬식을 계산합니다.
	temp[0] = Q[0] - v1[0];
	temp[1] = Q[1] - v1[1];
	temp[2] = Q[2] - v1[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// 외부에 있는지 확인하십시오.
	if (determinant > 0.001f)
	{
		return false;
	}

	// 세 번째 가장자리의 법선을 계산합니다.
	edgeNormal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
	edgeNormal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
	edgeNormal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

	// 행렬이 내부, 외부 또는 직접 가장자리에 있는지 결정하기 위해 행렬식을 계산합니다.
	temp[0] = Q[0] - v2[0];
	temp[1] = Q[1] - v2[1];
	temp[2] = Q[2] - v2[2];

	determinant = ((edgeNormal[0] * temp[0]) + (edgeNormal[1] * temp[1]) + (edgeNormal[2] * temp[2]));

	// 외부에 있는지 확인하십시오.
	if (determinant > 0.001f)
	{
		return false;
	}

	// 이제 우리 높이가 있습니다.
	height = Q[1];

	return true;
}

bool Terrain::InitializeComputeShader(ID3D11Device* device)
{
	HRESULT result;

	WCHAR CsFilename[] = L"../SystemClass/ComputePicking.hlsl";

	// 버텍스 쉐이더 코드를 컴파일한다.
	ID3D10Blob* CsBuffer = nullptr;

	result = D3DCompileFromFile(CsFilename, NULL, NULL, "CS", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &CsBuffer, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateComputeShader(CsBuffer->GetBufferPointer(), CsBuffer->GetBufferSize(), nullptr, &m_ComputeShader);
	if (FAILED(result))
		return false;

	CsBuffer->Release();

	if (!m_BrushBuffer)
	{
		D3D11_BUFFER_DESC BufferDesc;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.ByteWidth = sizeof(Brush);
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;

		 result = device->CreateBuffer(&BufferDesc, NULL, &m_BrushBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	if (!m_RayBuffer)
	{
		D3D11_BUFFER_DESC BufferDesc;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.ByteWidth = sizeof(RayBufferType);
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;

		 result = device->CreateBuffer(&BufferDesc, NULL, &m_RayBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	if (!m_ComputeInput)
	{
		ID3D11Buffer* buffer;

		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(InputDesc) * (m_IndexCount / 3);
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(InputDesc);

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_input;

		if (FAILED(device->CreateBuffer(&desc, &initData, &buffer)))
			return false;

		m_ComputeInput = (ID3D11Resource*)buffer;
	}

	if (!m_ComputeSRV)
	{
		ID3D11Buffer* buffer = (ID3D11Buffer*)m_ComputeInput;

		D3D11_BUFFER_DESC desc;
		buffer->GetDesc(&desc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.BufferEx.NumElements = (m_IndexCount / 3);

		if (FAILED(device->CreateShaderResourceView(buffer, &srvDesc, &m_ComputeSRV)))
			return false;
	}

	if (!m_ComputeOutput)
	{
		ID3D11Buffer* buffer;

		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(OutputDesc) * (m_IndexCount / 3);
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(OutputDesc);

		if (FAILED(device->CreateBuffer(&desc, nullptr, &buffer)))
			return false;

		m_ComputeOutput = (ID3D11Resource*)buffer;
	}

	if (!m_ComputeUAV)
	{
		ID3D11Buffer* buffer = (ID3D11Buffer*)m_ComputeOutput;

		D3D11_BUFFER_DESC desc;
		buffer->GetDesc(&desc);

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = (m_IndexCount / 3);

		if (FAILED(device->CreateUnorderedAccessView(buffer, &uavDesc, &m_ComputeUAV)))
			return false;
	}

	if (!m_Result)
	{
		ID3D11Buffer* buffer;

		D3D11_BUFFER_DESC desc = {};
		((ID3D11Buffer*)m_ComputeOutput)->GetDesc(&desc);
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.MiscFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		if (FAILED(device->CreateBuffer(&desc, nullptr, &buffer)))
			return false;

		m_Result = (ID3D11Resource*)buffer;
	}

	m_output = new OutputDesc[m_IndexCount / 3];

	return true;
}

bool Terrain::ComputePicking(ID3D11DeviceContext* devcon, XMFLOAT3* position)
{
	// Compute Shader Setting
	devcon->CSSetShader(m_ComputeShader, nullptr, 0);


	// RayBuffer Setting
	int x; int y;
	m_InputClass->GetMouseLocation(x, y);
	Ray ray = Ray::GetRay(x, y);

	m_MousePosition[0] = x;
	m_MousePosition[1] = y;


	D3D11_MAPPED_SUBRESOURCE subResource;
	if(FAILED(devcon->Map(m_RayBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource)))
		return false;

	RayBufferType* dataPtr = (RayBufferType*)subResource.pData;
	dataPtr->position = ray.m_Origin;
	dataPtr->size = m_IndexCount / 3;
	dataPtr->Direction = ray.m_Direction;

	devcon->Unmap(m_RayBuffer, 0);

	unsigned int bufferNumber = 0;
	devcon->CSSetConstantBuffers(bufferNumber, 1, &m_RayBuffer);


	// SRV, UAV Setting
	devcon->CSSetShaderResources(0, 1, &m_ComputeSRV);
	devcon->CSSetUnorderedAccessViews(0, 1, &m_ComputeUAV, nullptr);


	// Compute Picking
	int size = m_IndexCount / 3;
	UINT X = ceil((float)(size/ 1024.0f));
	devcon->Dispatch(X, 1, 1);


	// Get Result
	devcon->CopyResource(m_Result, m_ComputeOutput);
	if(FAILED(devcon->Map(m_Result, 0, D3D11_MAP_READ, 0, &subResource)))
		return false;
	//OutputDesc* dataPtr = (OutputDesc*)subResource.pData;

	memcpy((void*)m_output, subResource.pData, sizeof(OutputDesc) *  size);
	devcon->Unmap(m_Result, 0);

	float minDistance = FLT_MAX;
	int minIndex = -1;

	vector<OutputDesc> Vect;

	for (UINT i = 0; i < size; i++)
	{
		OutputDesc temp = m_output[i];
		if (temp.picked)
		{
			Vect.push_back(temp);
			if (minDistance > temp.distance)
			{
				minDistance = temp.distance;
				minIndex = i;
			}
		}
	}

	if (minIndex >= 0)
	{
		XMVECTOR origin = XMLoadFloat3(&ray.m_Origin);
		XMVECTOR direction = XMLoadFloat3(&ray.m_Direction);
		XMVECTOR pos = origin + direction * XMVectorReplicate(minDistance);
		XMFLOAT3 resultPos; XMStoreFloat3(&resultPos, pos);

		m_MouseWorldPosition[0] = resultPos.x;
		m_MouseWorldPosition[1] = resultPos.y;
		m_MouseWorldPosition[2] = resultPos.z;

		*position = resultPos;
		return true;
	}

	return false;
}

void Terrain::UpdateTerrainHeight(XMFLOAT3 position, float fDeltaTime)
{
	switch (m_Brush.type)
	{
	case 0:
	{
		for (int i = 0; i < m_VertexCount; i++)
		{
			 VertexType& vertex = m_Vertices[i];
			
			XMFLOAT3 p1 = XMFLOAT3(vertex.position.x, 0, vertex.position.z);
			XMFLOAT3 p2 = XMFLOAT3(position.x, 0, position.z);
			XMFLOAT3 temp = XMFLOAT3(p1.x - p2.x, 0, p1.z - p2.z);
			XMVECTOR tempVec = XMLoadFloat3(&temp);		
			float distance = XMVectorGetX(XMVector3Length(tempVec));
			
			float cosValue = cos(XM_PIDIV2 * distance / m_Brush.range);
			float value = m_AdjustValue * max(0, cosValue);
			
			if (distance <= m_Brush.range)
			{
				if (m_IsRaise)
					vertex.position.y += value * fDeltaTime;
				else
					vertex.position.y -= value * fDeltaTime;
			}
		}
		
	}
	break;
	case 1:
	{
		for (int i = 0; i < m_VertexCount; i++)
		{
			 VertexType& vertex = m_Vertices[i];
			XMFLOAT3 p1 = XMFLOAT3(vertex.position.x, 0, vertex.position.z);
			XMFLOAT3 p2 = XMFLOAT3(position.x, 0, position.z);
			
			float distX = abs(p2.x - p1.x);
			float distZ = abs(p2.z - p1.z);

			//
			if (distX <= m_Brush.range
				&& distZ <= m_Brush.range)
			{
				if (m_IsRaise)
					vertex.position.y += m_AdjustValue * fDeltaTime;
				else
					vertex.position.y -= m_AdjustValue * fDeltaTime;
			}
		}
	}
	break;
	default:
		break;
	}



	UpdateVertexBuffer(D3DClass::GetSingleton()->GetDeviceContext());
}

bool Terrain::UpdateVertexBuffer(ID3D11DeviceContext* devcon)
{
	devcon->UpdateSubresource(m_VertexBuffer, 0, nullptr, m_Vertices, sizeof(VertexType), m_VertexCount);
	
	return true;
}
