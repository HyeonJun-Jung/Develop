#include "TextureShaderClass.h"
#include "..\stdafx.h"

TextureShaderClass::TextureShaderClass()
{
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    WCHAR vs[] = L"../SystemClass/Texture.vs";
    WCHAR ps[] = L"../SystemClass/Texture.ps";

    return InitializeShader(device, hwnd, vs, ps);
}

void TextureShaderClass::Shutdown()
{
    ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatix, XMMATRIX viewMatrix,
                                XMMATRIX projectionMatirx, ID3D11ShaderResourceView* texture)
{
    if (!SetShaderParameters(deviceContext, worldMatix, viewMatrix, projectionMatirx, texture))
    {
        return false;
    }

    RenderShader(deviceContext);

    return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    //////////////////////////////
    // 정점 쉐이더, 픽셀 쉐이더 //
    //////////////////////////////

    //버택스 쉐이더 코드를 컴파일 한다.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
        0, &vertexShaderBuffer, &errorMessage);

    if (FAILED(result))
    {
        //셰이더 컴파일 실패 시 오류메세지 출력
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        else
        {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }
    }

    //버택스 쉐이더 코드를 컴파일 한다.
    ID3D10Blob* pixelShaderBuffer = nullptr;
    result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
        0, &pixelShaderBuffer, &errorMessage);

    if (FAILED(result))
    {
        //셰이더 컴파일 실패 시 오류메세지 출력
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        else
        {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }
    }

    //버퍼로부터 정점 쉐이더를 생성합니다.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
                                        NULL, &m_vertexShader);

    if (FAILED(result))
    {
        return false;
    }

    //버퍼로부터 픽셀 쉐이더를 생성합니다.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
                                        NULL, &m_pixelShader);

    if (FAILED(result))
    {
        return false;
    }


    ///////////////////
    // 정점 레이아웃 //
    ///////////////////


    // 정점 입력 레이아웃 구조체를 설정합니다.
    // 이 설정은 ModelClass와 쉐이더의 VertexType 구조와 일치해야 합니다.
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
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
    
    // 레이아웃의 요소 수를 가져옵니다.
    UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // 정점 입력 레이아웃을 만듭니다.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
                                       vertexShaderBuffer->GetBufferSize(), &m_layout);

    if (FAILED(result))
    {
        return false;
    }


    // 더 이상 사용되지 않는 정점 쉐이더 버퍼와 픽셀 쉐이더 버퍼를 해제합니다.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;


    ////////////////////
    // 행렬 상수 버퍼 //
    ////////////////////


    // 정점 쉐이더에 있는 행렬 상수 버퍼의 구조체를 작성합니다.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // 상수 버퍼 포인터를 만들어 이 클래스에서 정점 쉐이더 상수 버퍼에 접근할 수 있게 합니다.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result))
    {
        return false;
    }


    /////////////////
    // 텍스처 샘플 //
    /////////////////


    // 텍스처 샘플러 상태 구조체를 생성 및 설정합니다.
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        //;D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT D3D11_FILTER_MIN_MAG_MIP_LINEAR
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

    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void TextureShaderClass::ShutdownShader()
{
    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = 0;
    }

    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    if (m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    // 에러 메시지를 출력창에 표시합니다.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // 에러 메시지를 반환합니다.
    errorMessage->Release();
    errorMessage = 0;

    //컴파일 에러가 있음을 팝업 메시지로 알려줍니다.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);

}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
                                             XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
    // 행렬을 transpose하여 쉐이더에서 사용할 수 있도록 합니다.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }    

    // 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // 상수 버퍼에 행렬을 복사합니다.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // 상수 버퍼의 잠금을 풉니다.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // 정점 쉐이더에서의 상수 버퍼의 위치를 설정합니다.
    unsigned int bufferNumber = 0;

    // 마지막으로 정점 쉐이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    // 픽셀 쉐이더에서 쉐이더 텍스처 리소스를 설정합니다.
    deviceContext->PSSetShaderResources(0, 1, &texture);

    return true;
    
    
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext)
{
    // 정점 입력 레이아웃을 설정합니다.
    deviceContext->IASetInputLayout(m_layout);

    // 삼각형을 그릴 정점 쒜이더와 픽셀 쉐이더를 설정합니다.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // 픽셀 쉐이더에서 샘플러 상태를 설정합니다.
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);

}
