#pragma once
#include "GraphicComponent.h"
#include "..\stdafx.h"
class SkinnedObject;

class SkinnedGraphicC :
    public GraphicComponent
{
public:
    SkinnedGraphicC();
    virtual ~SkinnedGraphicC() {}

public:
    bool Initialize(ID3D11Device* dev);
    virtual bool Update(ID3D11DeviceContext* devcon, SkinnedObject* obj);
    

private:
    bool InitializeShader(ID3D11Device* dev);
    bool InitializeBuffer(ID3D11Device* dev);
    bool Render(ID3D11DeviceContext* devcon, SkinnedObject* obj);
    bool UpdateBuffer(ID3D11DeviceContext* devcon, SkinnedObject* obj);
  

private:
    static ID3D11VertexShader* m_VertexShader;
    static ID3D11PixelShader* m_PixelShader;
    static ID3D11InputLayout* m_layout;
    static ID3D11Buffer* m_CameraBuffer;
    static ID3D11SamplerState* m_sampleState;
};

