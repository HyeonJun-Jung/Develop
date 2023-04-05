#pragma once
#include "..\stdafx.h"
#include "Singleton.h"
#include "Flag.h"
#include "Shader.h"
class ShaderManager : public Singleton<ShaderManager>
{
public:
	ShaderManager();
	~ShaderManager();

private:
	void InsertShader(Resource_Type rt, Shader* shader);
	Shader* GetShader(Resource_Type rt);

private:
	map<Resource_Type, Shader*> ShaderMap;

};

