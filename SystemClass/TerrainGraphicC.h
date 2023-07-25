#pragma once
#include "GraphicComponent.h"
#include "..\stdafx.h"

class Terrain;
class TerrainGraphicC : public GraphicComponent
{
public:
	TerrainGraphicC();
	virtual ~TerrainGraphicC();

public:
	bool Initialize(ID3D11Device* dev);
	virtual bool Update(ID3D11DeviceContext* devcon, Terrain* terrain);

private:
	bool Render(ID3D11DeviceContext* devcon, Terrain* terrain);
	bool InitializeShader(ID3D11Device* dev);

private:
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11InputLayout* m_layout;

};

