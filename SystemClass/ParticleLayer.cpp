#include "ParticleLayer.h"
#include "Particle.h"

ParticleLayer::ParticleLayer(std::string layerName, int priority) : Layer(layerName, priority)
{
}

ParticleLayer::~ParticleLayer()
{
}

bool ParticleLayer::Initialize(ID3D11Device* dev)
{
    if (!Layer::Initialize(dev))
        return false;

    ParticleSystem* particle = new ParticleSystem();
    if (!particle)
        return false;
    if (!particle->Initialize(dev))
        return false;
    if (!particle->Create(dev))
        return false;

    m_ModelList.push_back(particle);

	return true;
}

bool ParticleLayer::Render(ID3D11DeviceContext* devcon)
{
    if (!Layer::Render(devcon))
        return false;
    return true;
}

bool ParticleLayer::PostRender(ID3D11DeviceContext* devcon)
{
    if (!Layer::PostRender(devcon))
        return false;
    return true;
}

bool ParticleLayer::UpdateAnimation(ID3D11DeviceContext* devcon, float fDeltaTime)
{
    if (!Layer::UpdateAnimation(devcon, fDeltaTime))
        return false;
    return true;
}

bool ParticleLayer::UpdatePhysics(ID3D11DeviceContext* devcon, float fDeltaTime)
{
    if (!Layer::UpdatePhysics(devcon, fDeltaTime))
        return false;
    return true;
}
