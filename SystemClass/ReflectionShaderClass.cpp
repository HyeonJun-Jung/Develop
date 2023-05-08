#include "ReflectionShaderClass.h"

#include <d3dcompiler.h>

ReflectionShaderClass::ReflectionShaderClass()
{
}

ReflectionShaderClass::ReflectionShaderClass(const ReflectionShaderClass&)
{
}

ReflectionShaderClass::~ReflectionShaderClass()
{
}

bool ReflectionShaderClass::Initialize(ID3D11Device* Device, HWND hwnd)
{
    WCHAR vs[] = L"../SystemClass/Reflection.vs";
    WCHAR ps[] = L"../SystemClass/Reflection.ps";

    if (!InitializeShader(Device, hwnd, vs, ps))
    {
        return false;
    }


    return true;
}

void ReflectionShaderClass::Shutdown()
{
    ShutdownShader();
}

bool ReflectionShaderClass::Render(ID3D11DeviceContext* DeviceContext, int indexCount, XMMATRIX WorldMatrix, XMMATRIX ViewMatrix, XMMATRIX ProjectionMatrix, 
                                    ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix)
{
    if (!SetShaderParameters(DeviceContext, WorldMatrix, ViewMatrix, ProjectionMatrix, Texture, reflectionTexture, reflectionMatrix))
    {
        return false;
    }

    RenderShader(DeviceContext);

    return true;
}

bool ReflectionShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;

	// 버텍스 쉐이더 코드를 컴파일한다.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ReflectionVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 쉐이더 코드를 컴파일한다.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ReflectionPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 정점 셰이더를 생성한다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버퍼에서 픽셀 쉐이더를 생성합니다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 입력 레이아웃 구조체를 설정합니다.
	// 이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야합니다.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// 레이아웃의 요소 수를 가져옵니다.
	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 만듭니다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제합니다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 텍스처 샘플러 상태 구조체를 생성 및 설정합니다.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처 샘플러 상태를 만듭니다.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 셰이더에 있는 행렬 상수 버퍼의 구조체를 작성합니다.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 셰이더에 들어가는 반사 행렬의 동적 상수 버퍼에 대한 description을 작성합니다.
	D3D11_BUFFER_DESC reflectionBufferDesc;
	reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	reflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
	reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	reflectionBufferDesc.MiscFlags = 0;
	reflectionBufferDesc.StructureByteStride = 0;

	// 버퍼를 생성하여 정점 셰이더의 상수 버퍼에 접근할 수 있게 합니다.
	result = device->CreateBuffer(&reflectionBufferDesc, NULL, &m_reflectionBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ReflectionShaderClass::ShutdownShader()
{ // 반사 상수 버퍼를 해제합니다.
    if (m_reflectionBuffer)
    {
        m_reflectionBuffer->Release();
        m_reflectionBuffer = 0;
    }

    // 샘플러를 해제합니다.
    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = 0;
    }

    // 행렬 상수 버퍼를 해제합니다.
    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    // 레이아웃을 해제합니다.
    if (m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // 픽셀 셰이더를 해제합니다.
    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // 정점 셰이더를 해제합니다.
    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }

    return;
}

void ReflectionShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	// 에러 메시지를 출력창에 표시합니다.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// 에러 메세지를 반환합니다.
	errorMessage->Release();
	errorMessage = 0;

	// 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool ReflectionShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX WorldMatrix, XMMATRIX ViewMatrix, XMMATRIX ProjectionMatrix,
											ID3D11ShaderResourceView* FloorTexture, ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix)
{
	// 행렬을 transpose하여 쉐이더에서 사용할 수 있도록 합니다.
	WorldMatrix = XMMatrixTranspose(WorldMatrix);
	ViewMatrix = XMMatrixTranspose(ViewMatrix);
	ProjectionMatrix = XMMatrixTranspose(ProjectionMatrix);
	reflectionMatrix = XMMatrixTranspose(reflectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->world = WorldMatrix;
	dataPtr->view = ViewMatrix;
	dataPtr->projection = ProjectionMatrix;

	// 상수 버퍼의 잠금을 풉니다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned int bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 기록할 수 있도록 반사 상수 버퍼를 잠급니다.	
	if (FAILED(deviceContext->Map(m_reflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 행렬 상수 버퍼의 데이터 포인터를 얻어옵니다.
	ReflectionBufferType* dataPtr2 = (ReflectionBufferType*)mappedResource.pData;

	// 행렬을 상수 버퍼에 복사합니다.
	dataPtr2->reflectionMatrix = reflectionMatrix;

	// 반사 상수 버퍼의 잠금을 해제합니다.
	deviceContext->Unmap(m_reflectionBuffer, 0);

	// 정점 셰이더상의 반사 상수 버퍼의 위치입니다.
	bufferNumber = 1;

	// 갱신된 값으로 정점 셰이더의 반사 행렬 상수 버퍼를 설정합니다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_reflectionBuffer);



	// 픽셀 셰이더에 텍스쳐를 설정합니다.
	deviceContext->PSSetShaderResources(0, 1, &FloorTexture);

	// 픽셀 셰이더에 반사 텍스쳐를 설정합니다.
	deviceContext->PSSetShaderResources(1, 1, &reflectionTexture);



	return true;
}

void ReflectionShaderClass::RenderShader(ID3D11DeviceContext* deviceContext)
{
	// 정점 입력 레이아웃을 설정합니다.
	deviceContext->IASetInputLayout(m_layout);

	// 정점 및 픽셀 셰이더를 설정합니다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 픽셀 셰이더의 샘플러를 설정합니다.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

}
