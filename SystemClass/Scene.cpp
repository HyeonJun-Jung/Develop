#include "Scene.h"
#include "Layer.h"
#include <algorithm>
#include "PhysicsComponent.h"
Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::Initialize(ID3D11Device* dev)
{
	return true;
}

bool Scene::Render(ID3D11DeviceContext* devcon)
{
	for (int i = 0; i < m_LayerList.size(); i++)
	{
		if (!m_LayerList[i]->Render(devcon))
			return false;
	}

	return true;
}

bool Scene::PostRender(ID3D11DeviceContext* devcon)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (int i = 0; i < m_LayerList.size(); i++)
	{
		if (!m_LayerList[i]->PostRender(devcon))
			return false;
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	// ImGui::EndFrame();
	return true;
}

bool Scene::Update(ID3D11DeviceContext* devcon, InputClass* input, float fDeltaTime)
{
	for (int i = 0; i < m_LayerList.size(); i++)
	{
		if (!m_LayerList[i]->Update(devcon, fDeltaTime))
			return false;
	}

	for (int i = 0; i < m_LayerList.size(); i++)
	{
		if (!m_LayerList[i]->UpdateInput(input, fDeltaTime))
			return false;
	}

	for(int i = 0; i < m_LayerList.size(); i++)
	{
		if (!m_LayerList[i]->UpdateAnimation(devcon, fDeltaTime))
			return false;
	}

	PhysicsComponent::UpdatePhysics(fDeltaTime);

	for (int i = 0; i < m_LayerList.size(); i++)
	{
		if (!m_LayerList[i]->UpdatePhysics(devcon, fDeltaTime))
			return false;
	}

	return true;
}



void Scene::InsertLayer(Layer* layer)
{
	m_LayerList.push_back(layer);
	sort(m_LayerList.begin(), m_LayerList.end(), Compare_Layer);
}

Layer* Scene::GetLayer(std::string layername)
{
	for (int i = 0; i < m_LayerList.size(); i++)
	{
		if (m_LayerList[i]->GetName().compare(layername) == 0)
			return m_LayerList[i];
	}
	return nullptr;
}

int Scene::Compare_Layer(Layer* layer1, Layer* layer2)
{
	return layer1->GetPriority() < layer2->GetPriority();
}
