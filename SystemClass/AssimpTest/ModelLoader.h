#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <map>
#include <string>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"
#include "TextureLoader.h"

using namespace std;
using namespace DirectX;


static float m_AnimationTime = 0;


struct TempVertexType {
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
};

struct TransformBufferType
{
	XMMATRIX TransformMatrix[200];
};

struct VectorType {
	float x, y, z;
};

struct boneInfo {
	XMMATRIX boneoffset;
	XMMATRIX finalTransform;
};

class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	bool Load(HWND hwnd, ID3D11Device* dev, ID3D11DeviceContext* devcon, float animationTime, std::string filename);
	void Draw(ID3D11DeviceContext* devcon);
	void RenderBuffers(ID3D11DeviceContext* devcon);

	bool ExtractBoneTransform(float animationTime, const int animationIndex);

	
	void Close();

	int m_vertexCount = 0;
	int m_indexCount = 0;
	int m_NumBones = 0;
	int m_NumVertex = 0;

	vector<XMMATRIX> m_boneTransforms;


private:
	ID3D11Device *dev_;
	ID3D11DeviceContext *devcon_;
	std::vector<Mesh> meshes_;
	std::string directory_;
	std::vector<Texture> textures_loaded_;
	HWND hwnd_;

	const aiScene* m_Scene;
	 aiNode m_RootNode;
	map<string, int> m_boneMap;
	vector<boneInfo> m_boneInfos;

	ID3D11Buffer* m_TransformBuffer;

	XMMATRIX m_GlobalInverseTransform;
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);

	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);
	void CalculateNumVertex(aiNode* node, const aiScene* scene);


	void ReadNodeHierarchy(float animationTime, const aiNode* node, const XMMATRIX& parentTransform);

	aiVector3D  CalcInterpolatedValueFromKey(float animationTime, const int numKeys, const aiVectorKey* const vectorKey) const;
	aiQuaternion  CalcInterpolatedValueFromKey(float animationTime, const int numKeys, const aiQuatKey* const quatKey) const;

	unsigned int FindKeyIndex(const float animationTime, const int numKeys, const aiVectorKey* const vectorKey) const;
	unsigned int FindKeyIndex(const float animationTime, const int numKeys, const aiQuatKey* const quatKey) const;

	aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const string nodeName);
	XMMATRIX ToMat4(aiMatrix4x4 mat);
};

#endif // !MODEL_LOADER_H

