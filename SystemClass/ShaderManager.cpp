#include "ShaderManager.h"
ShaderManager* ShaderManager::m_Instance = nullptr;
 
ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

void ShaderManager::InsertShader(Resource_Type rt, Shader* shader)
{
	ShaderMap.insert(make_pair(rt, shader));
}

Shader* ShaderManager::GetShader(Resource_Type rt)
{
	return ShaderMap[rt];
}

