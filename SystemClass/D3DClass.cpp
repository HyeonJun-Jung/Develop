#include "D3DClass.h"

D3DClass* D3DClass::m_Instance = nullptr;

D3DClass::D3DClass()
{
    m_swapChain = 0;
    m_device = 0;
    m_deviceContext = 0;
    m_renderTargetView = 0;
    m_depthStencilBuffer = 0;
    m_depthStencilState = 0;
    m_depthStencilView = 0;
    m_rasterState = 0;
    m_depthDisabledStencilState = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
    float screenDepth, float screenNear)
{
    m_Screenwidth = screenWidth;
    m_ScreenHeight = screenHeight;


    HRESULT result;
    int error;

    // vsync(��������ȭ) ������ �����մϴ�.
    m_vsync_enabled = vsync;

    // DirectX �׷��� �������̽� ���丮�� ����ϴ�.
    IDXGIFactory* factory;
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result))
    {
        return false;
    }

    // ���丮 ��ü�� ����Ͽ� ù��° �׷��� ī�� �������̽��� ���� �ƴ��͸� ����ϴ�.
    IDXGIAdapter* adapter;
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result))
    {
        return false;
    }

    // ���(�����)�� ���� ù��° �ƴ��͸� �����մϴ�.
    IDXGIOutput* adapterOutput;
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result))
    {
        return false;
    }

    // DXGI_FORMAT_R8G8B8A8_UNORM ����� ��� ���÷��� ���˿� �´� ����� ������ ���մϴ�.
    unsigned int numModes = 0;
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result))
    {
        return false;
    }

    // ������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ�� �����մϴ�.
    DXGI_MODE_DESC* displayModeList;
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList)
    {
        return false;
    }

    // ���÷��� ��忡 ���� ����Ʈ ������ ä���ֽ��ϴ�.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result))
    {
        return false;
    }

    // ���� ��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���̿� �´� ���÷��� ��带 ã���ϴ�.
    // ������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���� ���� �����մϴ�.
    unsigned int numerator = 0;
    unsigned int denominator = 0;
    for (unsigned int i = 0; i < numModes; i++)
    {
        if (displayModeList[i].Width == (unsigned int)screenWidth)
        {
            if (displayModeList[i].Height == (unsigned int)screenHeight)
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // �����(�׷���ī��)�� description�� �����ɴϴ�.
    DXGI_ADAPTER_DESC adapterDesc;
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result))
    {
        return false;
    }

    // ���� �׷���ī���� �޸� �뷮�� �ް�����Ʈ ������ �����մϴ�.
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // �׷���ī���� �̸��� char�� ���ڿ� �迭�� �ٲ� �� �����մϴ�.
    size_t stringLength;
    error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    if (error != 0)
    {
        return false;
    }

    // ���÷��� ��� ����Ʈ�� �Ҵ��� �����մϴ�.
    delete[] displayModeList;
    displayModeList = 0;

    // ��� �ƴ��͸� �Ҵ� �����մϴ�.
    adapterOutput->Release();
    adapterOutput = 0;

    // �ƴ��͸� �Ҵ� �����մϴ�.
    adapter->Release();
    adapter = 0;

    // ���丮 ��ü�� �Ҵ� �����մϴ�.
    factory->Release();
    factory = 0;





    // ���� ü�� description�� �ʱ�ȭ�մϴ�.
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // �ϳ��� ����۸��� ����ϵ��� �մϴ�.
    swapChainDesc.BufferCount = 1;

    // ������� �ʺ�� ���̸� �����մϴ�.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    // ����۷� �Ϲ����� 32bit�� �����̽��� �����մϴ�.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // ������� ���ΰ�ħ ������ �����մϴ�.
    if (m_vsync_enabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // ������� �뵵�� �����մϴ�.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // �������� �̷���� �������� �ڵ��� �����մϴ�.
    swapChainDesc.OutputWindow = hwnd;

    // ��Ƽ���ø��� ���ϴ�.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // ������ ��� �Ǵ� Ǯ��ũ�� ��带 �����մϴ�.
    if (fullscreen)
    {
        swapChainDesc.Windowed = false;
    }
    else
    {
        swapChainDesc.Windowed = true;
    }

    // ��ĵ������ ���İ� ��ĵ���̴��� �������� ��������(unspecified) �����մϴ�.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // ��µ� ������ ������� ������ �������� �մϴ�.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // �߰� �ɼ� �÷��׸� ������� �ʽ��ϴ�.
    swapChainDesc.Flags = 0;

    // ���� ������ DirectX 11�� �����մϴ�.
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    // ���� ü��, Direct3D ����̽�, Direct3D ����̽� ���ؽ�Ʈ�� �����մϴ�.
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if (FAILED(result))
    {
        return false;
    }

    // ������� �����͸� �����ɴϴ�.
    ID3D11Texture2D* backBufferPtr;
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result))
    {
        return false;
    }

    // ������� �����ͷ� ����Ÿ�� �並 �����մϴ�.
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if (FAILED(result))
    {
        return false;
    }

    // ����� �����͸� ���̻� ������� �����Ƿ� �Ҵ� �����մϴ�.
    backBufferPtr->Release();
    backBufferPtr = 0;

    // ���� ������ description�� �ʱ�ȭ�մϴ�.
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // ���� ������ description�� �ۼ��մϴ�.
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // description�� ����Ͽ� ����-���ٽ� ������ �ؽ��ĸ� �����մϴ�.
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // ���ٽ� ������ description�� �ʱ�ȭ�մϴ�.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // ���ٽ� ������ description�� �ۼ��մϴ�.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // �ȼ� ������ ���ٽ� �����Դϴ�.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // �ȼ� �޸��� ���ٽ� �����Դϴ�.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // ����-���ٽ� ���¸� �����մϴ�.
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if (FAILED(result))
    {
        return false;
    }

    // ����-���ٽ� ���¸� �����մϴ�.
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

    // ����-���ٽ� ���� description�� �ʱ�ȭ�մϴ�.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // ����-���ٽ� ���� description�� �ۼ��մϴ�.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // ����-���ٽ� �並 �����մϴ�.
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(result))
    {
        return false;
    }

    // ����Ÿ�� ��� ����-���ٽ� ���۸� ���� ��� ���������ο� ���ε��մϴ�.
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    
    

    // � ������ ��� �׸� ������ �����ϴ� ������ȭ�� description�� �ۼ��մϴ�.
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // �ۼ��� description���κ��� ������ȭ�� ���¸� �����մϴ�.
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
    if (FAILED(result))
    {
        return false;
    }

    // ��� �ø��� ���� �ʴ� ������ ���� ����
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateNoCulling);
    if (FAILED(result))
        return false;

    // ������ȭ�� ���¸� �����մϴ�.
    m_deviceContext->RSSetState(m_rasterState);

    // �������� ���� ����Ʈ�� �����մϴ�.
    m_viewport;
    m_viewport.Width = (float)screenWidth;
    m_viewport.Height = (float)screenHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;

    // ����Ʈ�� �����մϴ�.
    m_deviceContext->RSSetViewports(1, &m_viewport);
    
    // ���� ����� �����մϴ�.
    float fieldOfView, screenAspect;
    fieldOfView = (float)XM_PI / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    // 3D �������� ���� ���� ����� �����մϴ�.
    m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

    // ���� ����� ���� ��ķ� �ʱ�ȭ�մϴ�.
    m_worldMatrix = XMMatrixIdentity();

    // 2D �������� ���� ���� ���� ����� �����մϴ�.
    m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);




    // 2D �������� ���� Z���۸� �� ������ ���¸� ����ϴ�.
    // ������ �������� DepthEnalbe�� false ��� ���Դϴ�.
    D3D11_DEPTH_STENCIL_DESC depthDisalbedStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // ���ٽ� ������ description�� �ۼ��մϴ�.
    depthDisalbedStencilDesc.DepthEnable = false;
    depthDisalbedStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisalbedStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthDisalbedStencilDesc.StencilEnable = true;
    depthDisalbedStencilDesc.StencilReadMask = 0xFF;
    depthDisalbedStencilDesc.StencilWriteMask = 0xFF;

    // �ȼ� ������ ���ٽ� �����Դϴ�.
    depthDisalbedStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisalbedStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisalbedStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisalbedStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // �ȼ� �޸��� ���ٽ� �����Դϴ�.
    depthDisalbedStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisalbedStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisalbedStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisalbedStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // ����-���ٽ� ���¸� �����մϴ�.
    result = m_device->CreateDepthStencilState(&depthDisalbedStencilDesc, &m_depthDisabledStencilState);
    if (FAILED(result))
    {
        return false;
    }

    // ���� ���⸦ ���� �ʴ� ����
    depthDisalbedStencilDesc.DepthEnable = true;
    depthDisalbedStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

    result = m_device->CreateDepthStencilState(&depthDisalbedStencilDesc, &m_depthWriteFalseState);
    if (FAILED(result))
    {
        return false;
    }   


    // ������ ���� ���� ���¸� ����ϴ�.
    D3D11_BLEND_DESC blendStateDescription;
    ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
    
    // Create an alpha enabled blend state description
    blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
    blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    // Create the blend state using the description.
    result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
    if (FAILED(result))
    {
        return false;
    }

    blendStateDescription.AlphaToCoverageEnable = false;
    blendStateDescription.IndependentBlendEnable = false;

    blendStateDescription.RenderTarget[0].BlendEnable = true;
    blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    result = m_device->CreateBlendState(&blendStateDescription, &m_ParticleState);
    if (FAILED(result))
    {
        return false;
    }

    // Modify the description to create an alpha disabled blend state description.
    blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

    result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void D3DClass::Shutdown()
{
    // �����ϱ� ���� �̷��� ������ ���� �ٲ��� ������ ����ü���� �Ҵ� ������ �� ���ܰ� �߻��մϴ�.
    if (m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    if (m_alphaDisableBlendingState)
    {
        m_alphaDisableBlendingState->Release();
        m_alphaDisableBlendingState = 0;
    }

    if (m_alphaDisableBlendingState)
    {
        m_alphaDisableBlendingState->Release();
        m_alphaDisableBlendingState = 0;
    }


    if (m_alphaEnableBlendingState)
    {
        m_alphaEnableBlendingState->Release();
        m_alphaEnableBlendingState = 0;
    }

    if (m_rasterState)
    {
        m_rasterState->Release();
        m_rasterState = 0;
    }

    if (m_rasterStateNoCulling)
    {
        m_rasterStateNoCulling->Release();
        m_rasterStateNoCulling = 0;
    }

    if (m_depthStencilView)
    {
        m_depthStencilView->Release();
        m_depthStencilView = 0;
    }

    if (m_depthStencilState)
    {
        m_depthStencilState->Release();
        m_depthStencilState = 0;
    }

    if (m_depthStencilBuffer)
    {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = 0;
    }

    if (m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = 0;
    }

    if (m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = 0;
    }

    if (m_device)
    {
        m_device->Release();
        m_device = 0;
    }

    if (m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = 0;
    }

    return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];


    // ���۸� � �������� ���� ������ �����մϴ�.
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // ������� ������ ����ϴ�.
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    // ���� ���۸� ����ϴ�.
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    return;
}


void D3DClass::EndScene()
{
    // �������� �Ϸ�Ǿ����Ƿ� ������� ������ ȭ�鿡 ǥ���մϴ�.
    if (m_vsync_enabled)
    {
        // ���ΰ�ħ ������ �����մϴ�.
        m_swapChain->Present(1, 0);
    }
    else
    {
        // ������ �� ������ ǥ���մϴ�.
        m_swapChain->Present(0, 0);
    }

    return;
}

ID3D11Device* D3DClass::GetDevice()
{
    return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
    return m_deviceContext;
}

ID3D11DepthStencilView* D3DClass::GetDepthStencilView()
{
    return m_depthStencilView;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
    projectionMatrix = m_projectionMatrix;
    return;
}


void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
    worldMatrix = m_worldMatrix;
    return;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
    orthoMatrix = m_orthoMatrix;
    return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
    return;
}

void D3DClass::TurnZBufferOn()
{
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void D3DClass::TurnZBufferOff()
{
    m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
}

void D3DClass::EnableDepthMask()
{
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void D3DClass::DisableDepthMask()
{
    m_deviceContext->OMSetDepthStencilState(m_depthWriteFalseState, 1);
}

void D3DClass::TurnOnAlphaBlending()
{
    float blendFactor[4];

    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void D3DClass::TurnOffAlphaBlending()
{
    float blendFactor[4];

    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}

void D3DClass::EnableParticleBlending()
{
    float blendFactor[4];

    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    m_deviceContext->OMSetBlendState(m_ParticleState, blendFactor, 0xffffffff);
}

void D3DClass::TurnOnCulling()
{
    m_deviceContext->RSSetState(m_rasterState);
}

void D3DClass::TurnOffCulling()
{
    m_deviceContext->RSSetState(m_rasterStateNoCulling);
}

void D3DClass::ResetViewport()
{
    // ����Ʈ�� �缳���մϴ�.
    m_deviceContext->RSSetViewports(1, &m_viewport);
}

void D3DClass::SetBackBufferRenderTarget()
{
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}
