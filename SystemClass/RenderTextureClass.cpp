#include "RenderTextureClass.h"

RenderTextureClass::RenderTextureClass()
{
}

RenderTextureClass::RenderTextureClass(const RenderTextureClass&)
{
}

RenderTextureClass::~RenderTextureClass()
{
}

bool RenderTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT result;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


    // RTT 디스크립션을 초기화합니다.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // RTT 디스크립션을 세팅합니다.
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // RTT를 생성합니다.
    result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
    if (FAILED(result))
    {
        return false;
    }

    // 렌더 타겟 뷰에 대한 디스크립션을 설정합니다.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // 렌더 타겟 뷰를 생성합니다.
    result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
    if (FAILED(result))
    {
        return false;
    }

    // 셰이더 리소스 뷰에 대한 디스크립션을 설정합니다.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // 셰이더 리소스 뷰를 생성합니다.
    result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void RenderTextureClass::Shutdown()
{
    if (m_shaderResourceView)
    {
        m_shaderResourceView->Release();
        m_shaderResourceView = 0;
    }

    if (m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = 0;
    }

    if (m_renderTargetTexture)
    {
        m_renderTargetTexture->Release();
        m_renderTargetTexture = 0;
    }

}

void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* DeviceContext, ID3D11DepthStencilView* DepthStencilView)
{
    DeviceContext->OMSetRenderTargets(1, &m_renderTargetView, DepthStencilView);
}

void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext* DeviceContext, ID3D11DepthStencilView* DepthStencilView, float r, float g, float b, float a)
{ 
    float color[4] = { r, g, b, a };

    // 백버퍼를 초기화합니다.
    DeviceContext->ClearRenderTargetView(m_renderTargetView, color);

    // 깊이버퍼를 초기화합니다.
    DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView()
{
    return m_shaderResourceView;
}
