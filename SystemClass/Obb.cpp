#include "Obb.h"
#include "D3DClass.h"
#include "ShaderManager.h"
Obb::Obb()
{
}

Obb::Obb(const Obb& Obb)
{
}

Obb::~Obb()
{
}

bool Obb::SetObb(XMFLOAT3 min, XMFLOAT3 max)
{
	MinPos = min;
	MaxPos = max;

	Center = XMFLOAT3((MaxPos.x - MinPos.x) / 2, (MaxPos.y - MinPos.y) / 2, (MaxPos.z - MinPos.z) / 2);

	CreateCube();
	CreateAxisVector();
	
	if (!CreateBuffer())
		return false;

	return true;
}

void Obb::Render()
{
}

void Obb::CreateCube()
{
	XMFLOAT3 Position;
	XMFLOAT4 Color = XMFLOAT4(1.0, 1.0, 1.0, 1.0);
	
	// Obb의 전면
	// 왼쪽 아래
	Position = MinPos;
	Cube[0] = Vertex(Position, Color);
	// 왼쪽 위
	Position = XMFLOAT3(MinPos.x, MaxPos.y, MinPos.z);
	Cube[1] = Vertex(Position, Color);
	// 오른쪽 위
	Position = XMFLOAT3(MaxPos.x, MaxPos.y, MinPos.z);
	Cube[2] = Vertex(Position, Color);
	// 오른쪽 아래
	Position = XMFLOAT3(MaxPos.x, MinPos.y, MinPos.z);
	Cube[3] = Vertex(Position, Color);

	// Obb의 뒷면
	// 왼쪽 아래
	Position = XMFLOAT3(MinPos.x, MinPos.y, MaxPos.z);
	Cube[4] = Vertex(Position, Color);
	// 왼쪽 위
	Position = XMFLOAT3(MinPos.x, MaxPos.y, MaxPos.z);
	Cube[5] = Vertex(Position, Color);
	// 오른쪽 위
	Position = MaxPos;
	Cube[6] = Vertex(Position, Color);
	// 오른쪽 아래
	Position = XMFLOAT3(MaxPos.x, MinPos.y, MaxPos.z);
	Cube[7] = Vertex(Position, Color);

	unsigned int indice[] =
	{
		// 앞면
		0, 1, 2,
		0, 2, 3,

		// 뒷면
		4, 6, 5,
		4, 7, 6,

		// 왼쪽 면
		4, 5, 1,
		4, 1, 0,

		// 오른쪽 면
		3, 2, 6,
		3, 6, 7,

		// 윗면
		1, 5, 6,
		1, 6, 2,

		// 아랫면
		4, 0, 3,
		4, 3, 7
	};

	for (int i = 0; i < 36; i++)
		indices[i] = indice[i];
}

void Obb::CreateAxisVector()
{
	XMFLOAT3 dir(1.0f, 0.0f, 0.0f);
	AxisUnitVec[0] = XMLoadFloat3(&dir);
	dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
	AxisUnitVec[1] = XMLoadFloat3(&dir);
	dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
	AxisUnitVec[2] = XMLoadFloat3(&dir);

	AxisHalfLength[0] = MaxPos.x - Center.x;
	AxisHalfLength[1] = MaxPos.y - Center.y;
	AxisHalfLength[2] = MaxPos.z - Center.z;
}

bool Obb::CreateBuffer()
{
	ID3D11Device* dev = D3DClass::GetSingleton()->GetDevice();
	
	HRESULT hr;
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * 8);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &Cube[0];
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	hr = dev->CreateBuffer(&vbd, &initData, &VertexBuffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create Obb vertex buffer.");
		return false;
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * 36);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	initData.pSysMem = &indices[0];
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;


	hr = dev->CreateBuffer(&ibd, &initData, &IndexBuffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create Obb index buffer.");
	}

	return true;
}
