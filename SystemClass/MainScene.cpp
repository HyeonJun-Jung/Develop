#include "MainScene.h"
#include "Layer.h"
#include "Player.h"
#include "SkydomeClass.h"
#include "D3DClass.h"
#include "FieldLayer.h"
#include "ParticleLayer.h"
#include "..\stdafx.h"
MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

bool MainScene::Initialize(ID3D11Device* dev)
{
	
	/*
	FieldLayer* Field = new FieldLayer("Field", 1);
	if (!Field)
		return false;
	if (!Field->Initialize(dev))
		return false;

	m_LayerList.push_back(Field);
	*/
	
	ParticleLayer* particleLayer = new ParticleLayer("Particle", 1);
	if (!particleLayer)
		return false;
	if (!particleLayer->Initialize(dev))
		return false;
		
	m_LayerList.push_back(particleLayer);
	
	/*
	Layer* defaultLayer = GetLayer("Default");
	if (defaultLayer)
	{

		Player* player = new Player();
		if (!player)
			return false;

		if (!player->Initialize("../SystemClass/data/Standard Walk.fbx"))
			return false;

		player->PlayAnimation(0);

		//player->SetScale(0.05, 0.05, 0.05);

		defaultLayer->InsertModel(player);
	}


	Layer* backGroundLayer = GetLayer("BackGround");
	if (backGroundLayer)
	{
		SkydomeClass* skydome = new SkydomeClass();
		if (!skydome)
			return false;

		if (!skydome->Initialize(D3DClass::GetSingleton()->GetDevice()))
			return false;

		backGroundLayer->InsertModel(skydome);
	}
	*/

	return true;
}

bool MainScene::Render(ID3D11DeviceContext* devcon)
{
	if (!Scene::Render(devcon))
		return false;
	return true;
}

bool MainScene::PostRender(ID3D11DeviceContext* devcon)
{
	if (!Scene::PostRender(devcon))
		return false;
	return true;
}

bool MainScene::Update(ID3D11DeviceContext* devcon, InputClass* input, float fDeltaTime)
{
	if (!Scene::Update(devcon, input, fDeltaTime))
		return false;
	return true;
}
