#include "TerrainGraphicC.h"
#include "Terrain.h"

ID3D11VertexShader* TerrainGraphicC::m_VertexShader = nullptr;
ID3D11PixelShader* TerrainGraphicC::m_PixelShader = nullptr;
ID3D11InputLayout* TerrainGraphicC::m_layout = nullptr;


TerrainGraphicC::TerrainGraphicC()
{
}

TerrainGraphicC::~TerrainGraphicC()
{
}

bool TerrainGraphicC::Initialize(ID3D11Device* dev)
{
	if (!GraphicComponent::Initialize(dev))
		return false;

	if (!InitializeShader(dev))
		return false;

	return true;
}

bool TerrainGraphicC::Update(ID3D11DeviceContext* devcon, Terrain* terrain)
{
	if (!GraphicComponent::Update(devcon, terrain))
		return false;

	if (!Render(devcon, terrain))
		return false;

	return true;
}

bool TerrainGraphicC::Render(ID3D11DeviceContext* devcon, Terrain* terrain)
{
	devcon->VSSetShader(m_VertexShader, NULL, 0);
	devcon->PSSetShader(m_PixelShader, NULL, 0);

	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	ID3D11Buffer* vertexBuffer = terrain->GetVertexBuffer();
	ID3D11Buffer* indexBuffer = terrain->GetIndexBuffer();

	devcon->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->IASetInputLayout(m_layout);

	ID3D11ShaderResourceView* texture = terrain->GetTexture();
	devcon->PSSetShaderResources(0, 1, &texture);

	devcon->DrawIndexed(terrain->GetIndexCount(), 0, 0);

	return true;
}

bool TerrainGraphicC::InitializeShader(ID3D11Device* dev)
{
	if (!m_VertexShader)
	{
		HRESULT result;
		ID3D10Blob* errorMessage = nullptr;
		HWND hwnd = GetConsoleWindow();

		WCHAR vsFilename[] = L"../SystemClass/Terrain_vs.hlsl";
		// 버텍스 쉐이더 코드를 컴파일한다.
		ID3D10Blob* vertexShaderBuffer = nullptr;

		result = D3DCompileFromFile(vsFilename, NULL, NULL, "TerrainVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
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

		WCHAR psFilename[] = L"../SystemClass/Terrain_ps.hlsl";

		// 픽셀 쉐이더 코드를 컴파일한다.
		ID3D10Blob* pixelShaderBuffer = nullptr;
		result = D3DCompileFromFile(psFilename, NULL, NULL, "TerrainPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
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

		UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

		if (FAILED(dev->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
		{
			return false;
		}
	}

	return true;
}
