#include "SkydomeShaderClass.h"

SkydomeShaderClass::SkydomeShaderClass()
{
}

SkydomeShaderClass::SkydomeShaderClass(const SkydomeShaderClass&)
{
}

SkydomeShaderClass::~SkydomeShaderClass()
{
}

bool SkydomeShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	WCHAR vs[] = L"../SystemClass/skydome_vs.hlsl";
	WCHAR ps[] = L"../SystemClass/skydome_ps.hlsl";
	if (!InitializeShader(device, hwnd, vs, ps))
		return false;

	return true;
}

void SkydomeShaderClass::Shutdown()
{
	ShutdownShader();
}

bool SkydomeShaderClass::Render(ID3D11DeviceContext* devcon, int indexCount, XMMATRIX WorldMatrix, XMMATRIX ViewMatrix, XMMATRIX ProjectionMatrix, XMFLOAT4 apex, XMFLOAT4 center)
{
	if (!SetShaderParameters(devcon, WorldMatrix, ViewMatrix, ProjectionMatrix, apex, center))
		return false;

	RenderShader(devcon);

	return true;
}

bool SkydomeShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	ID3D10Blob* errorMessage = nullptr ;

	// 버텍스 쉐이더 코드를 컴파일한다.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFileName, NULL, NULL, "SkyDomeVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 쉐이더 코드를 컴파일한다.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFileName, NULL, NULL, "SkyDomePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))
	{
		return false;
	}

	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
				vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	D3D11_BUFFER_DESC MatrixBufferDesc; 
	MatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	MatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBufferDesc.MiscFlags = 0;
	MatrixBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&MatrixBufferDesc, NULL, &m_matrixBuffer)))
	{
		return false;
	}

	D3D11_BUFFER_DESC GradientBufferDesc;
	GradientBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	GradientBufferDesc.ByteWidth = sizeof(GradientBufferType);
	GradientBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	GradientBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GradientBufferDesc.MiscFlags = 0;
	GradientBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&GradientBufferDesc, NULL, &m_GradientBuffer)))
	{
		return false;
	}

	return true;
}

void SkydomeShaderClass::ShutdownShader()
{
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_GradientBuffer)
	{
		m_GradientBuffer->Release();
		m_GradientBuffer = 0;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}
}

void SkydomeShaderClass::OutputShaderErrorMessage(ID3D10Blob* ErrorMessage, HWND hwnd, WCHAR* FileName)
{
	// 에러 메시지를 출력창에 표시합니다.
	OutputDebugStringA(reinterpret_cast<const char*>(ErrorMessage->GetBufferPointer()));

	// 에러 메세지를 반환합니다.
	ErrorMessage->Release();
	ErrorMessage = 0;

	// 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
	MessageBox(hwnd, L"Error compiling shader.", FileName, MB_OK);
}

bool SkydomeShaderClass::SetShaderParameters(ID3D11DeviceContext* devcon, XMMATRIX WorldMatrix, XMMATRIX ViewMatrix, XMMATRIX ProjectionMatrix, XMFLOAT4 apex, XMFLOAT4 center)
{
	WorldMatrix = XMMatrixTranspose(WorldMatrix);
	ViewMatrix = XMMatrixTranspose(ViewMatrix);
	ProjectionMatrix = XMMatrixTranspose(ProjectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_matrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	MatrixBufferType* dataptr = (MatrixBufferType*) mappedResource.pData;

	dataptr->world = WorldMatrix;
	dataptr->view = ViewMatrix;
	dataptr->projection = ProjectionMatrix;

	devcon->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	
	if (FAILED(devcon->Map(m_GradientBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;

	GradientBufferType* dataptr2 = (GradientBufferType*)mappedResource.pData;

	dataptr2->apexColor = apex;
	dataptr2->centerColor = center;

	devcon->Unmap(m_GradientBuffer, 0);

	bufferNumber = 0;

	devcon->PSSetConstantBuffers(bufferNumber, 1, &m_GradientBuffer);

	return true;
}

void SkydomeShaderClass::RenderShader(ID3D11DeviceContext* devcon)
{
	// 정점 입력 레이아웃을 설정합니다.
	devcon->IASetInputLayout(m_layout);

	// 정점 및 픽셀 셰이더를 설정합니다.
	devcon->VSSetShader(m_vertexShader, NULL, 0);
	devcon->PSSetShader(m_pixelShader, NULL, 0);

}
