#include "ModelLoader.h"

ModelLoader::ModelLoader() :
        dev_(nullptr),
        devcon_(nullptr),
        meshes_(),
        directory_(),
        textures_loaded_(),
        hwnd_(nullptr) {
    // empty
}


ModelLoader::~ModelLoader() {
    // empty
}

bool ModelLoader::Load(HWND hwnd, ID3D11Device * dev, ID3D11DeviceContext * devcon, float animationTime, std::string filename) {
	Assimp::Importer importer;

	m_Scene = importer.ReadFile(filename,
		aiProcess_Triangulate | aiProcess_GenSmoothNormals |
		aiProcess_ConvertToLeftHanded | aiProcess_FlipUVs);
	

	this->directory_ = filename.substr(0, filename.find_last_of("/\\"));

	this->dev_ = dev;
	this->devcon_ = devcon;
	this->hwnd_ = hwnd;

	m_RootNode = *m_Scene->mRootNode;

	m_GlobalInverseTransform = XMMatrixInverse(nullptr, XMMATRIX(&m_Scene->mRootNode->mTransformation.a1));

	//CalculateNumVertex(m_Scene->mRootNode, m_Scene);

	processNode(m_Scene->mRootNode, m_Scene);

	for (int i = 0; i < m_boneInfos.size(); i++)
	{
		m_boneInfos[i].finalTransform = XMMatrixIdentity();
	}

	ExtractBoneTransform(20.47446, 0);

	// 버텍스 쉐이더에있는 변환 행렬 상수 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC TransformBufferDesc;
	TransformBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	TransformBufferDesc.ByteWidth = sizeof(TransformBufferType);
	TransformBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	TransformBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TransformBufferDesc.MiscFlags = 0;
	TransformBufferDesc.StructureByteStride = 0;

	// 이 클래스 내에서 정점 셰이더 상수 버퍼에 액세스 할 수 있도록 변환 행렬 상수 버퍼 포인터를 만듭니다.
	if (FAILED(dev->CreateBuffer(&TransformBufferDesc, NULL, &m_TransformBuffer)))
	{
		return false;
	}

	// 쓸 수 있도록 카메라 상수 버퍼를 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (FAILED(devcon->Map(m_TransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	TransformBufferType* dataPtr3 = (TransformBufferType*)mappedResource.pData;

	// 최종 변환 행렬을 상수 버퍼에 복사합니다.
	for (int i = 0; i < m_boneTransforms.size(); i++)
	{
		dataPtr3->TransformMatrix[i] = m_boneTransforms[i];
	}

	// 카메라 상수 버퍼를 잠금 해제합니다.
	devcon->Unmap(m_TransformBuffer, 0);

	// 버텍스 쉐이더에서 카메라 상수 버퍼의 위치를 ​​설정합니다.
	int bufferNumber = 2;

	// 이제 업데이트 된 값으로 버텍스 쉐이더에서 카메라 상수 버퍼를 설정합니다.
	devcon->VSSetConstantBuffers(bufferNumber, 1, &m_TransformBuffer);


	return true;
}

void ModelLoader::Draw(ID3D11DeviceContext * devcon) {
	for (size_t i = 0; i < meshes_.size(); ++i ) {
		meshes_[i].Draw(devcon);
	}
}

void ModelLoader::RenderBuffers(ID3D11DeviceContext* devcon)
{
	for (size_t i = 0; i < meshes_.size(); ++i)
	{
		meshes_[i].RenderBuffers(devcon);
	}
}


Mesh ModelLoader::processMesh(aiMesh * mesh, const aiScene * scene) {
	// Data to fill
	std::vector<VERTEX> vertices;
	std::vector<UINT> indices;
	std::vector<Texture> textures;

	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		VERTEX vertex;

		vertex.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		
		
		if (mesh->mTextureCoords[0]) {
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertex.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		vertices.push_back(vertex);
	}

	// tangent, binormal 계산

	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;

	for (UINT i = 0; i < mesh->mNumFaces; i++) {

		aiFace face = mesh->mFaces[i];
		int index1 = face.mIndices[0];
		int index2 = face.mIndices[1];
		int index3 = face.mIndices[2];


		vertex1.x = mesh->mVertices[index1].x;
		vertex1.y = mesh->mVertices[index1].y;
		vertex1.z = mesh->mVertices[index1].z;
		if (mesh->mTextureCoords[0]) {
			vertex1.tu = (float)mesh->mTextureCoords[0][index1].x;
			vertex1.tv = (float)mesh->mTextureCoords[0][index1].y;
		}
		vertex1.nx = mesh->mNormals[index1].x;
		vertex1.ny = mesh->mNormals[index1].y;
		vertex1.nz = mesh->mNormals[index1].z;


		vertex2.x = mesh->mVertices[index2].x;
		vertex2.y = mesh->mVertices[index2].y;
		vertex2.z = mesh->mVertices[index2].z;
		if (mesh->mTextureCoords[0]) {
			vertex2.tu = (float)mesh->mTextureCoords[0][index2].x;
			vertex2.tv = (float)mesh->mTextureCoords[0][index2].y;
		}
		vertex2.nx = mesh->mNormals[index2].x;
		vertex2.ny = mesh->mNormals[index2].y;
		vertex2.nz = mesh->mNormals[index2].z;


		vertex3.x = mesh->mVertices[index3].x;
		vertex3.y = mesh->mVertices[index3].y;
		vertex3.z = mesh->mVertices[index3].z;
		if (mesh->mTextureCoords[0]) {
			vertex3.tu = (float)mesh->mTextureCoords[0][index3].x;
			vertex3.tv = (float)mesh->mTextureCoords[0][index3].y;
		}
		vertex3.nx = mesh->mNormals[index3].x;
		vertex3.ny = mesh->mNormals[index3].y;
		vertex3.nz = mesh->mNormals[index3].z;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		vertices[index1].normal.x = normal.x;
		vertices[index1].normal.y = normal.y;
		vertices[index1].normal.z = normal.z;
		vertices[index1].tangent.x = tangent.x;
		vertices[index1].tangent.y = tangent.y;
		vertices[index1].tangent.z = tangent.z;
		vertices[index1].binormal.x = binormal.x;
		vertices[index1].binormal.y = binormal.y;
		vertices[index1].binormal.z = binormal.z;

		vertices[index2].normal.x = normal.x;
		vertices[index2].normal.y = normal.y;
		vertices[index2].normal.z = normal.z;
		vertices[index2].tangent.x = tangent.x;
		vertices[index2].tangent.y = tangent.y;
		vertices[index2].tangent.z = tangent.z;
		vertices[index2].binormal.x = binormal.x;
		vertices[index2].binormal.y = binormal.y;
		vertices[index2].binormal.z = binormal.z;

		vertices[index3].normal.x = normal.x;
		vertices[index3].normal.y = normal.y;
		vertices[index3].normal.z = normal.z;
		vertices[index3].tangent.x = tangent.x;
		vertices[index3].tangent.y = tangent.y;
		vertices[index3].tangent.z = tangent.z;
		vertices[index3].binormal.x = binormal.x;
		vertices[index3].binormal.y = binormal.y;
		vertices[index3].binormal.z = binormal.z;


		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Bone과 Vertex 연결	
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		unsigned int boneIndex;
		string BoneName(mesh->mBones[i]->mName.data);

		if (m_boneMap.find(BoneName) == m_boneMap.end())
		{
			boneIndex = m_NumBones++;
			boneInfo bi;
			// bi.boneoffset = XMMATRIX(mesh->mBones[i]->mOffsetMatrix[0]);
			bi.boneoffset = XMMatrixTranspose(XMMATRIX(mesh->mBones[i]->mOffsetMatrix[0]));
			m_boneInfos.push_back(bi);
		}
		else
		{
			boneIndex = m_boneMap[BoneName];
		}

		m_boneMap[BoneName] = boneIndex;

		

		for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{

			unsigned int vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			

				if (vertices[vertexId].BoneWeights.x == 0.0f)
				{
					vertices[vertexId].BoneIds.x = boneIndex;
					vertices[vertexId].BoneWeights.x = weight;
					
				}
				else if (vertices[vertexId].BoneWeights.y == 0.0f)
				{
					vertices[vertexId].BoneIds.y = boneIndex;
					vertices[vertexId].BoneWeights.y = weight;
				
				}
				else if (vertices[vertexId].BoneWeights.z == 0.0f)
				{
					vertices[vertexId].BoneIds.z = boneIndex;
					vertices[vertexId].BoneWeights.z = weight;
				
				}
				else if (vertices[vertexId].BoneWeights.w == 0.0f)
				{
					vertices[vertexId].BoneIds.w = boneIndex;
					vertices[vertexId].BoneWeights.w = weight;	
					
				}

		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", scene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> glossinessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_Glossiness", scene);
		textures.insert(textures.end(), glossinessMaps.begin(), glossinessMaps.end());
		// 5. Emissive
		std::vector<Texture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_Emissive", scene);
		textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
	}
	
	return Mesh(dev_, vertices, indices, textures);
}

void ModelLoader::CalculateNumVertex(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_NumVertex += mesh->mNumVertices;
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		this->CalculateNumVertex(node->mChildren[i], scene);
	}

	
}

std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName, const aiScene * scene) {
	std::vector<Texture> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (UINT j = 0; j < textures_loaded_.size(); j++) {
			if (std::strcmp(textures_loaded_[j].path.c_str(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded_[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip) {   // If texture hasn't been loaded already, load it
			HRESULT hr;
			Texture texture;

			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr) {
				texture.texture = loadEmbeddedTexture(embeddedTexture);
			} else {
				std::string filename = std::string(str.C_Str());
				filename = directory_ + '/' + filename;
				std::wstring filenamews = std::wstring(filename.begin(), filename.end());
				hr = CreateWICTextureFromFile(dev_, devcon_, filenamews.c_str(), nullptr, &texture.texture);
				if (FAILED(hr))
					MessageBox(hwnd_, L"Texture couldn't be loaded", L"Error!", MB_ICONERROR | MB_OK);
			}
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			this->textures_loaded_.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void ModelLoader::Close() {
	for (auto& t : textures_loaded_)
		t.Release();

	for (size_t i = 0; i < meshes_.size(); i++) {
		meshes_[i].Close();
	}
}

void ModelLoader::processNode(aiNode * node, const aiScene * scene) {
	int maxMeshes = 0;
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		if (node->mNumMeshes > maxMeshes)
			maxMeshes = node->mNumMeshes;
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes_.push_back(this->processMesh(mesh, scene));
		m_NumVertex += mesh->mNumVertices;
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}

	//int temp = meshes_.size();

}

ID3D11ShaderResourceView * ModelLoader::loadEmbeddedTexture(const aiTexture* embeddedTexture) {
	HRESULT hr;
	ID3D11ShaderResourceView *texture = nullptr;

	if (embeddedTexture->mHeight != 0) {
		// Load an uncompressed ARGB8888 embedded texture
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = embeddedTexture->mWidth;
		desc.Height = embeddedTexture->mHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = embeddedTexture->pcData;
		subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;
		subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;

		ID3D11Texture2D *texture2D = nullptr;
		hr = dev_->CreateTexture2D(&desc, &subresourceData, &texture2D);
		if (FAILED(hr))
			MessageBox(hwnd_, L"CreateTexture2D failed!", L"Error!", MB_ICONERROR | MB_OK);

		hr = dev_->CreateShaderResourceView(texture2D, nullptr, &texture);
		if (FAILED(hr))
			MessageBox(hwnd_, L"CreateShaderResourceView failed!", L"Error!", MB_ICONERROR | MB_OK);

		return texture;
	}

	// mHeight is 0, so try to load a compressed texture of mWidth bytes
	const size_t size = embeddedTexture->mWidth;

	hr = CreateWICTextureFromMemory(dev_, devcon_, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture);
	if (FAILED(hr))
		MessageBox(hwnd_, L"Texture couldn't be created from memory!", L"Error!", MB_ICONERROR | MB_OK);
	
	return texture;
}

void ModelLoader::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
	VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void ModelLoader::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	float length;


	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}


bool ModelLoader::ExtractBoneTransform(float animationTime, const int animationIndex)
{
	// 애니메이션이 계속 반복되도록 fmod 연산을 취함
	animationTime = fmod(animationTime, m_Scene->mAnimations[animationIndex]->mDuration);
	// root node와 단위 행렬을 인자로 넘겨주면 재귀 호출을 통하여 boneInfos에 데이터를 저장하는 함수
	ReadNodeHierarchy(animationTime, m_Scene->mRootNode, XMMatrixIdentity());

	m_boneTransforms.resize(m_NumBones);

	for (int i = 0; i < m_NumBones; i++)
	{
		// boneTransforms는 vector<mat4> 타입으로 크기는 Bone의 개수와 같음
		m_boneTransforms[i] = m_boneInfos[i].finalTransform;
	}

	return true;
}

void ModelLoader::ReadNodeHierarchy(float animationTime, const aiNode* node, const XMMATRIX& parentTransform)
{

	string nodeName(node->mName.data);

	const aiAnimation* animation = m_Scene->mAnimations[0];
	XMMATRIX globalTransform = XMMatrixIdentity();
		
	XMMATRIX nodeTransform = XMMATRIX(&node->mTransformation.a1);
	nodeTransform = XMMatrixTranspose(nodeTransform);

	const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);

	if (nodeAnim)
	{	
		const aiVector3D& scaling = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumScalingKeys, nodeAnim->mScalingKeys);
		XMMATRIX scalingM = XMMatrixScaling(scaling.x, scaling.y, scaling.z);
		

		const aiQuaternion& rotationQ = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumRotationKeys, nodeAnim->mRotationKeys);
		 XMMATRIX rotationM = XMMatrixRotationQuaternion(XMVectorSet(rotationQ.w, rotationQ.x, rotationQ.y, rotationQ.z));
		//XMMATRIX rotationM = XMMatrixRotationQuaternion(XMVectorSet(rotationQ.x, rotationQ.y, rotationQ.z, rotationQ.w));
	

		const aiVector3D& translation = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumPositionKeys, nodeAnim->mPositionKeys);
		XMMATRIX translationM = XMMatrixTranslation(translation.x, translation.y, translation.z);
		
		
		nodeTransform = scalingM * rotationM * translationM;
		//nodeTransform = XMMatrixTranspose(nodeTransform);
	}

	//nodeTransform = XMMATRIX(&node->mTransformation.a1);
	
	globalTransform = nodeTransform * parentTransform;
	//globalTransform = parentTransform * nodeTransform;
	
	if (m_boneMap.find(nodeName) != m_boneMap.end())
	{
		unsigned int boneIndex = m_boneMap[nodeName];
		
		// boneOffset은 model space에서 정의되었던 정점들을 bone space에서 정의되도록 만드는 것
		//m_boneInfos[boneIndex].finalTransform = m_GlobalInverseTransform * globalTransform * m_boneInfos[boneIndex].boneoffset;
		 m_boneInfos[boneIndex].finalTransform = m_boneInfos[boneIndex].boneoffset * globalTransform * m_GlobalInverseTransform;	
		 //m_boneInfos[boneIndex].finalTransform = m_boneInfos[boneIndex].boneoffset * parentTransform * nodeTransform;
		 //m_boneInfos[boneIndex].finalTransform = XMMatrixTranspose(m_boneInfos[boneIndex].finalTransform);
	}
	
	for (int i = 0; i < node->mNumChildren; i++)
		ReadNodeHierarchy(animationTime, node->mChildren[i], globalTransform);
	
}


aiVector3D ModelLoader::CalcInterpolatedValueFromKey(float animationTime, const int numKeys, const aiVectorKey* const vectorKey) const
{
	aiVector3D ret;

	if (numKeys == 1)
	{
		ret = vectorKey[0].mValue;
		return ret;
	}

	unsigned int keyIndex = FindKeyIndex(animationTime, numKeys, vectorKey);
	unsigned int nextKeyIndex = keyIndex + 1;

	assert(nextKeyIndex < numKeys);

	float deltaTime = vectorKey[nextKeyIndex].mTime - vectorKey[keyIndex].mTime;
	float factor = (animationTime - (float)vectorKey[keyIndex].mTime) / deltaTime;

	assert(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D& startValue = vectorKey[keyIndex].mValue;
	const aiVector3D& endValue = vectorKey[nextKeyIndex].mValue;

	ret.x = startValue.x + (endValue.x - startValue.x) * factor;
	ret.y = startValue.y + (endValue.y - startValue.y) * factor;
	ret.z = startValue.z + (endValue.z - startValue.z) * factor;

	return ret;
}

aiQuaternion ModelLoader::CalcInterpolatedValueFromKey(float animationTime, const int numKeys, const aiQuatKey* const quatKey) const
{
	aiQuaternion ret;
	if (numKeys == 1)
	{
		ret = quatKey[0].mValue;
		return ret;
	}


	unsigned int keyIndex = FindKeyIndex(animationTime, numKeys, quatKey);
	unsigned int nextKeyIndex = keyIndex + 1;

	assert(nextKeyIndex < numKeys);

	float deltaTime = quatKey[nextKeyIndex].mTime - quatKey[keyIndex].mTime;
	float factor = (animationTime - (float)quatKey[keyIndex].mTime) / deltaTime;

	assert(factor >= 0.0f && factor <= 1.0f);

	const aiQuaternion& startValue = quatKey[keyIndex].mValue;
	const aiQuaternion& endValue = quatKey[nextKeyIndex].mValue;
	aiQuaternion::Interpolate(ret, startValue, endValue, factor);
	ret = ret.Normalize();

	return ret;
}

unsigned int ModelLoader::FindKeyIndex(const float animationTime, const int numKeys, const aiVectorKey* const vectorKey) const
{
	assert(numKeys > 0);
	for (int i = 0; i < numKeys - 1; i++)
		if (animationTime < (float)vectorKey[i + 1].mTime)
			return i;
	assert(0);
}

unsigned int ModelLoader::FindKeyIndex(const float animationTime, const int numKeys, const aiQuatKey* const quatKey) const
{
	assert(numKeys > 0);
	for (int i = 0; i < numKeys - 1; i++)
		if (animationTime < (float)quatKey[i + 1].mTime)
			return i;
	assert(0);
}



aiNodeAnim* ModelLoader::FindNodeAnim(const aiAnimation* animation, const string nodeName)
{
	for (int i = 0; i < animation->mNumChannels; i++)
		if (animation->mChannels[i]->mNodeName.data == nodeName)
			return animation->mChannels[i];
	return nullptr;
}



XMMATRIX ModelLoader::ToMat4(aiMatrix4x4 mat)
{
	XMMATRIX nodeTransform(mat.a1, mat.a2, mat.a3, mat.a4,
		mat.b1, mat.b2, mat.b3, mat.b4,
		mat.c1, mat.c2, mat.c3, mat.c4,
		mat.d1, mat.d2, mat.d3, mat.d4);
	return nodeTransform;
}