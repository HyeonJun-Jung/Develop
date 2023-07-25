#include "SkinnedGraphicC.h"
#include "buffer.h"
#include "SkinnedObject.h"
#include "SkinnedModel.h"
#include "CameraClass.h"
ID3D11VertexShader* SkinnedGraphicC::m_VertexShader = nullptr;
ID3D11PixelShader* SkinnedGraphicC::m_PixelShader = nullptr;
ID3D11InputLayout* SkinnedGraphicC::m_layout = nullptr;
ID3D11Buffer* SkinnedGraphicC::m_CameraBuffer = nullptr;
ID3D11SamplerState* SkinnedGraphicC::m_sampleState = nullptr;

SkinnedGraphicC::SkinnedGraphicC()
{
	
}

bool SkinnedGraphicC::Initialize(ID3D11Device* dev)
{
	if (!GraphicComponent::Initialize(dev))
		return false;

	if (!InitializeShader(dev))
		return false;

	if (!InitializeBuffer(dev))
		return false;

    return true;
}

bool SkinnedGraphicC::Update(ID3D11DeviceContext* devcon, SkinnedObject* obj)
{
	if (!GraphicComponent::Update(devcon, obj))
		return false;

	if (!UpdateBuffer(devcon, obj))
		return false;

	if (!Render(devcon, obj))
		return false;

	return true;
}

bool SkinnedGraphicC::Render(ID3D11DeviceContext* devcon, SkinnedObject* obj)
{
	devcon->IASetInputLayout(m_layout);
	devcon->VSSetShader(m_VertexShader, NULL, 0);
	devcon->PSSetShader(m_PixelShader, NULL, 0);
	devcon->PSSetSamplers(0, 1, &m_sampleState);

	obj->GetSkinnedModel()->Render(devcon);

	return true;
}

bool SkinnedGraphicC::UpdateBuffer(ID3D11DeviceContext* devcon, SkinnedObject* obj)
{
	XMFLOAT3 cameraPos = CameraClass::GetSingleton()->GetPosition();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	CameraBufferType* dataPtr = (CameraBufferType*)mappedResource.pData;

	dataPtr->cameraPosition = cameraPos;
	dataPtr->padding = 0;

	devcon->Unmap(m_CameraBuffer, 0);

	unsigned int bufferNumber = 1;

	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_CameraBuffer);
	return true;
}

bool SkinnedGraphicC::InitializeShader(ID3D11Device* dev)
{
	if (!m_VertexShader)
	{
		HRESULT result;
		ID3D10Blob* errorMessage = nullptr;
		HWND hwnd = GetConsoleWindow();

		WCHAR vsFilename[] = L"../SystemClass/Bumpmap.vs";

		// 버텍스 쉐이더 코드를 컴파일한다.
		ID3D10Blob* vertexShaderBuffer = nullptr;

		result = D3DCompileFromFile(vsFilename, NULL, NULL, "BumpMapVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
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


		// 버퍼로부터 정점 셰이더를 생성한다.
		result = dev->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
		if (FAILED(result))
		{
			return false;
		}

		WCHAR psFilename[] = L"../SystemClass/Bumpmap.ps";
		// 픽셀 쉐이더 코드를 컴파일한다.
		ID3D10Blob* pixelShaderBuffer = nullptr;
		result = D3DCompileFromFile(psFilename, NULL, NULL, "BumpMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
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
		
		// 버퍼에서 픽셀 쉐이더를 생성합니다.
		result = dev->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
		if (FAILED(result))
		{
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC polygonLayout[7];
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

		polygonLayout[3].SemanticName = "TANGENT";
		polygonLayout[3].SemanticIndex = 0;
		polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[3].InputSlot = 0;
		polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[3].InstanceDataStepRate = 0;

		polygonLayout[4].SemanticName = "BINORMAL";
		polygonLayout[4].SemanticIndex = 0;
		polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[4].InputSlot = 0;
		polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[4].InstanceDataStepRate = 0;

		polygonLayout[5].SemanticName = "BLENDWEIGHT";
		polygonLayout[5].SemanticIndex = 0;
		polygonLayout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[5].InputSlot = 0;
		polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[5].InstanceDataStepRate = 0;

		polygonLayout[6].SemanticName = "BLENDINDICES";
		polygonLayout[6].SemanticIndex = 0;
		polygonLayout[6].Format = DXGI_FORMAT_R32G32B32A32_UINT;
		polygonLayout[6].InputSlot = 0;
		polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[6].InstanceDataStepRate = 0;

		// 레이아웃의 요소 수를 가져옵니다.
		UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

		// 정점 입력 레이아웃을 만듭니다.
		result = dev->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), &m_layout);
		if (FAILED(result))
		{
			return false;
		}

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
		result = dev->CreateSamplerState(&samplerDesc, &m_sampleState);
		if (FAILED(result))
		{
			return false;
		}
	}

	
	return true;
}

bool SkinnedGraphicC::InitializeBuffer(ID3D11Device* dev)
{
	if (!m_CameraBuffer)
	{
		D3D11_BUFFER_DESC BufferDesc;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.ByteWidth = sizeof(CameraBufferType);
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;

		HRESULT result = dev->CreateBuffer(&BufferDesc, NULL, &m_CameraBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	return true;
}
