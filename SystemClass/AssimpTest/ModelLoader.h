#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include "..\..\stdafx.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"
#include "TextureLoader.h"
#include "..\SkinnedModel.h"

using namespace std;
using namespace DirectX;

class SkinnedModel;

struct TempVertexType {
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
};

class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	SkinnedModel* Load(HWND hwnd, ID3D11Device* dev, ID3D11DeviceContext* devcon, float animationTime, std::string filename);
	bool AddAnimation(HWND hwnd, ID3D11Device* dev, ID3D11DeviceContext* devcon, SkinnedModel* model, std::string filename);
	void Close();

	int m_vertexCount = 0;
	int m_indexCount = 0;
	int m_NumBones = 0;
	int m_NumVertex = 0;
	XMFLOAT3 Maxpos = XMFLOAT3(-1000, -1000, -1000);
	XMFLOAT3 Minpos = XMFLOAT3(1000, 1000, 1000);

private:
	HWND hwnd_;
	ID3D11Device *dev_;
	ID3D11DeviceContext *devcon_;
	std::string directory_;
	std::vector<Texture> textures_loaded_;

	vector<PxVec3> px_vertices;
	vector<PxU8> px_indices;
private:

	void processNode(aiNode* ainode, NodeInfo* node);
	void processMesh(aiMesh* mesh, const aiScene* scene, SkinnedModel* model);
	void processAnimation(const aiScene* scene, SkinnedModel* model);
	void processPhysxGeometry(SkinnedModel* model);


	 std::  vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);

	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);
	void CalculateNumVertex(aiNode* node, const aiScene* scene);
	XMMATRIX ToMat4(aiMatrix4x4 mat);

	bool CreateVertexBuffer(const vector<VERTEX>& vertices, ID3D11Buffer** vertexBuffer);
	bool CreateIndexBuffer(const vector<unsigned int>& indices, ID3D11Buffer** IndexBuffer);
};

#endif // !MODEL_LOADER_H

