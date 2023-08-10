#include "UILayer.h"

UILayer::UILayer() : Layer("UI", 7)
{
}

bool UILayer::Initialize(ID3D11Device* dev)
{

    return true;
}



bool UILayer::Render(ID3D11DeviceContext* devcon)
{
    if (!Layer::Render(devcon))
        return false;
    return true;
}

bool UILayer::PostRender(ID3D11DeviceContext* devcon)
{
    if (!Layer::PostRender(devcon))
        return false;
    return true;
}

bool UILayer::UpdateAnimation(ID3D11DeviceContext* devcon, float fDeltaTime)
{
    if (!Layer::UpdateAnimation(devcon, fDeltaTime))
        return false;
    return true;
}

bool UILayer::UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime)
{
    if (!Layer::UpdatePhysics(devcon, fDeltaTime))
        return false;
    return true;
}