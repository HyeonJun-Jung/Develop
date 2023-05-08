#pragma once
#include "..\stdafx.h"
#include "Shader.h"
class SkydomeShaderClass : public Shader
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct GradientBufferType
    {
        XMFLOAT4 apexColor;
        XMFLOAT4 centerColor;
    };

public:
    SkydomeShaderClass();
    SkydomeShaderClass(const SkydomeShaderClass&);
    ~SkydomeShaderClass();

    bool Initialize(ID3D11Device* device, HWND hwnd);
    void Shutdown();
    bool Render(ID3D11DeviceContext* devcon, int indexCount, XMMATRIX WorldMatrix, XMMATRIX ViewMatrix, XMMATRIX ProjectionMatrix, XMFLOAT4 apex, XMFLOAT4 center);

private:
    bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* ErrorMessage, HWND hwnd, WCHAR* FileName);

    bool SetShaderParameters(ID3D11DeviceContext* devcon, XMMATRIX WorldMatrix, XMMATRIX ViewMatrix, XMMATRIX ProjectionMatrix, XMFLOAT4 apex, XMFLOAT4 center);
    void RenderShader(ID3D11DeviceContext* devcon);

private:
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11Buffer* m_GradientBuffer = nullptr;
};

