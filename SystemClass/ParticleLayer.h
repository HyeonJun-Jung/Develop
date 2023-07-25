#pragma once
#include "Layer.h"
class ParticleLayer :
    public Layer
{
public:
    ParticleLayer(std::string layerName, int priority);
    virtual ~ParticleLayer();

public:
    bool Initialize(ID3D11Device* dev);
    virtual bool Render(ID3D11DeviceContext* devcon);
    virtual bool PostRender(ID3D11DeviceContext* devcon);
    virtual bool UpdateAnimation(ID3D11DeviceContext* devcon, float fDeltaTime);
    virtual bool UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime);
};

