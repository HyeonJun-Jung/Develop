#pragma once
#include "..\stdafx.h"
class Object;

class GraphicComponent
{
public:
    GraphicComponent() {}
    virtual ~GraphicComponent() {}

public:
    virtual bool Update(ID3D11DeviceContext* devcon, Object* obj);
    virtual bool Initialize(ID3D11Device* dev);
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

private:
    virtual bool UpdateWorldMatrix(ID3D11DeviceContext* devcon, Object* obj);

private:
    static ID3D11Buffer* m_MatrixBuffer;
};
