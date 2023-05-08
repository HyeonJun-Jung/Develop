#pragma once
#include "..\stdafx.h"
//
// 다형성을 위한 Shader클래스
//
class Shader
{
public:
	Shader();
	~Shader();

public:
	virtual bool Initialize(ID3D11Device*, HWND) = 0;
	virtual void Shutdown() = 0;
	virtual void RenderShader(ID3D11DeviceContext* devcon) = 0;
};
