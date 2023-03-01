#include "FontShaderClass.h"

FontShaderClass::FontShaderClass()
{
}

FontShaderClass::FontShaderClass(const FontShaderClass&)
{
}

FontShaderClass::~FontShaderClass()
{
}

bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{

	// 정점 및 인덱스 쉐이더를 초기화합니다.
	WCHAR vsFilename[] = L"../SystemClass/Font.vs";
	WCHAR psFilename[] = L"../SystemClass/Font.ps";

	if (!InitializeShader(device, hwnd, vsFilename, psFilename))
	{
		return false;
	}

	return true;
}

void FontShaderClass::Shutdown()
{
	ShutdownShader();
}

bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
				XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatix, 
				ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{
	// 렌더링에 사용할 매개 변수들을 설정합니다.
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatix, texture, pixelColor))
	{
		return false;
	}

	// 설정된 버퍼 쉐이더로 렌더링합니다.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    //////////////////////////////
    // 정점 쉐이더, 픽셀 쉐이더 //
    //////////////////////////////

    //버택스 쉐이더 코드를 컴파일 한다.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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
    result = D3DCompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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
    D3D11_BUFFER_DESC ConstantBufferDesc;
    ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    ConstantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
    ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ConstantBufferDesc.MiscFlags = 0;
    ConstantBufferDesc.StructureByteStride = 0;

    // 상수 버퍼 포인터를 만들어 이 클래스에서 정점 쉐이더 상수 버퍼에 접근할 수 있게 합니다.
    result = device->CreateBuffer(&ConstantBufferDesc, NULL, &m_constantBuffer);
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


    // Setup the description of the dynamic pixel constant buffer that is in the pixel shader.
    D3D11_BUFFER_DESC pixelBufferDesc;
    pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
    pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    pixelBufferDesc.MiscFlags = 0;
    pixelBufferDesc.StructureByteStride = 0;

    // Create the pixel constant buffer pointer so we can access the pixel shader constant buffer from within this class.
    result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void FontShaderClass::ShutdownShader()
{
    // Release the pixel constant buffer.
    if (m_pixelBuffer)
    {
        m_pixelBuffer->Release();
        m_pixelBuffer = 0;
    }

    // Release the sampler state.
    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = 0;
    }

    // Release the constant buffer.
    if (m_constantBuffer)
    {
        m_constantBuffer->Release();
        m_constantBuffer = 0;
    }

    // Release the layout.
    if (m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // Release the pixel shader.
    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // Release the vertex shader.
    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }

    return;
}

void FontShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    char* compileErrors;
    unsigned long bufferSize, i;
    ofstream fout;


    // Get a pointer to the error message text buffer.
    compileErrors = (char*)(errorMessage->GetBufferPointer());

    // Get the length of the message.
    bufferSize = errorMessage->GetBufferSize();

    // Open a file to write the error message to.
    fout.open("shader-error.txt");

    // Write out the error message.
    for (i = 0; i < bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    // Close the file.
    fout.close();

    // Release the error message.
    errorMessage->Release();
    errorMessage = 0;

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

    return;
}

bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
                XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{
    HRESULT result;

    // 행렬을 transpose하여 쉐이더에서 사용할 수 있도록 합니다.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
    ConstantBufferType* dataPtr = (ConstantBufferType*)mappedResource.pData;

    // 상수 버퍼에 행렬을 복사합니다.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // 상수 버퍼의 잠금을 풉니다.
    deviceContext->Unmap(m_constantBuffer, 0);

    // 정점 쉐이더에서의 상수 버퍼의 위치를 설정합니다.
    unsigned int bufferNumber = 0;

    // 마지막으로 정점 쉐이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);




    // 픽셀 쉐이더에서 쉐이더 텍스처 리소스를 설정합니다.
    deviceContext->PSSetShaderResources(0, 1, &texture);




    // Lock the pixel constant buffer so it can be written to.
    result = deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    PixelBufferType* dataPtr2 = (PixelBufferType*)mappedResource.pData;

    // Get a pointer to the data in the pixel constant buffer.
    dataPtr2 = (PixelBufferType*)mappedResource.pData;

    // Copy the pixel color into the pixel constant buffer.
    dataPtr2->pixelColor = pixelColor;

    // Unlock the pixel constant buffer.
    deviceContext->Unmap(m_pixelBuffer, 0);

    // Set the position of the pixel constant buffer in the pixel shader.
    bufferNumber = 0;

    // Now set the pixel constant buffer in the pixel shader with the updated value.
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);

    return true;
}

void FontShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // 정점 입력 레이아웃을 설정합니다.
    deviceContext->IASetInputLayout(m_layout);

    // 삼각형을 그릴 정점 쒜이더와 픽셀 쉐이더를 설정합니다.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // 픽셀 쉐이더에서 샘플러 상태를 설정합니다.
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);

    //삼각형을 그립니다 .
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
