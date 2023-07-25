#include "ModelLoader.h"
#include "..\SkinnedModel.h"
#include "..\PhysicsComponent.h"


ModelLoader::ModelLoader() :
        dev_(nullptr),
        devcon_(nullptr),
        directory_(),
        textures_loaded_(),
        hwnd_(nullptr) {
    // empty
}


ModelLoader::~ModelLoader() {
    // empty
}

SkinnedModel* ModelLoader::Load(HWND hwnd, ID3D11Device * dev, ID3D11DeviceContext * devcon, float animationTime, std::string filename) {
	Assimp::Importer importer;

	const aiScene * scene = importer.ReadFile(filename,
		aiProcess_Triangulate | aiProcess_GenSmoothNormals |
		aiProcess_ConvertToLeftHanded | aiProcess_LimitBoneWeights);

	if (!scene)
		return nullptr;

	this->directory_ = filename.substr(0, filename.find_last_of("/\\"));

	this->dev_ = dev;
	this->devcon_ = devcon;
	this->hwnd_ = hwnd;

	SkinnedModel* model = new SkinnedModel;

	NodeInfo * node = model->GetRootNode();

	processNode(scene->mRootNode, node);

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		processMesh(scene->mMeshes[i], scene, model);
	}

	if(scene->HasAnimations())
		processAnimation(scene, model);

	processPhysxGeometry(model);

	*model->GetMaxPos() = XMFLOAT3(Maxpos.x, Maxpos.y, Maxpos.z);
	*model->GetMinPos() = XMFLOAT3(Minpos.x, Minpos.y, Minpos.z);

	px_vertices.clear();
	px_indices.clear();

	return model;
}

bool ModelLoader::AddAnimation(HWND hwnd, ID3D11Device* dev, ID3D11DeviceContext* devcon, SkinnedModel* model, std::string filename)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_Triangulate | aiProcess_GenSmoothNormals |
		aiProcess_ConvertToLeftHanded | aiProcess_LimitBoneWeights);

	if (!scene)
		return false;

	if (scene->HasAnimations())
		processAnimation(scene, model);

	return true;
}

void ModelLoader::processMesh(aiMesh * mesh, const aiScene * scene, SkinnedModel* model) {
	MtMesh* Mtmesh = new MtMesh;

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

		if (mesh->mVertices[i].x > Maxpos.x) Maxpos.x = mesh->mVertices[i].x;
		if (mesh->mVertices[i].y > Maxpos.y) Maxpos.y = mesh->mVertices[i].y;
		if (mesh->mVertices[i].z > Maxpos.z) Maxpos.z = mesh->mVertices[i].z;
		if (mesh->mVertices[i].x < Minpos.x) Minpos.x = mesh->mVertices[i].x;
		if (mesh->mVertices[i].y < Minpos.y) Minpos.y = mesh->mVertices[i].y;
		if (mesh->mVertices[i].z < Minpos.z) Minpos.z = mesh->mVertices[i].z;

		vertices.push_back(vertex);

		px_vertices.push_back(PxVec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
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
		{
			if (face.mNumIndices != 3)
				continue;
			indices.push_back(face.mIndices[j]);
			px_indices.push_back(face.mIndices[j]);
		}
	}
	
	vector<boneInfo>* BoneList = model->GetBoneList();
	map<string, int>* BoneMap = model->GetBoneMap();
	// Bone과 Vertex 연결	
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		unsigned int boneIndex;
		string BoneName(mesh->mBones[i]->mName.data);

		if (BoneMap->find(BoneName) == BoneMap->end())
		{
			boneIndex = m_NumBones++;
			boneInfo bi;
			// bi.boneoffset = XMMATRIX(mesh->mBones[i]->mOffsetMatrix[0]);
			bi.boneoffset = XMMatrixTranspose( XMMATRIX(&mesh->mBones[i]->mOffsetMatrix.a1));
			BoneList->push_back(bi);
		}
		else
		{
			boneIndex = (*BoneMap)[BoneName];
		}
		BoneMap->insert(make_pair(BoneName, boneIndex));
//		m_boneMap[BoneName] = boneIndex;


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
	
	ID3D11Buffer** VertexBuffer = &Mtmesh->VertexBuffer;
	CreateVertexBuffer(vertices, VertexBuffer);
	Mtmesh->VertexCount = vertices.size();

	ID3D11Buffer** IndexBuffer = &Mtmesh->IndexBuffer;
	CreateIndexBuffer(indices, IndexBuffer);
	Mtmesh->IndexCount = indices.size();

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		aiString  texture_path[15];
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path[0], NULL, NULL, NULL, NULL, NULL);
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_SPECULAR, 0, &texture_path[1], NULL, NULL, NULL, NULL, NULL);
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_AMBIENT, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_EMISSIVE, 0, &texture_path[3], NULL, NULL, NULL, NULL, NULL);
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_HEIGHT, 0, &texture_path[5], NULL, NULL, NULL, NULL, NULL);
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_NORMALS, 0, &texture_path[6], NULL, NULL, NULL, NULL, NULL);
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_SHININESS, 0, &texture_path[7], NULL, NULL, NULL, NULL, NULL);
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_OPACITY, 0, &texture_path[8], NULL, NULL, NULL, NULL, NULL);
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DISPLACEMENT, 0, &texture_path[9], NULL, NULL, NULL, NULL, NULL);
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texture_path[10], NULL, NULL, NULL, NULL, NULL);

		
		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", scene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. glossiness maps
		std::vector<Texture> glossinessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_Glossiness", scene);
		textures.insert(textures.end(), glossinessMaps.begin(), glossinessMaps.end());
		// 5. Emissive
		std::vector<Texture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_Emissive", scene);
		textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
		// 6. height 
		std::vector<Texture> HeightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", scene);
		textures.insert(textures.end(), HeightMaps.begin(), HeightMaps.end());
	}

	for (int i = 0; i < textures.size(); i++)
	{
		Marterial mt;
		mt.path = textures[i].path;
		mt.type = textures[i].type;
		mt.texture = textures[i].texture;

		if (mt.type == "texture_diffuse")
			model->Mt_Type.Diffuse = true;

		else if (mt.type == "texture_specular")
			model->Mt_Type.Specular = true;

		else if (mt.type == "texture_normal")
			model->Mt_Type.Normal = true;

		else if (mt.type == "texture_Glossiness")
			model->Mt_Type.Glossiness = true;

		else if (mt.type == "texture_Emissive")
			model->Mt_Type.Emissive = true;

		Mtmesh->Textures.push_back(mt);
	}

	model->GetMeshList()->push_back(Mtmesh);
	
	// MeshList.push_back(Mesh(dev_, vertices, indices, textures));
}

void ModelLoader::processAnimation(const aiScene* scene, SkinnedModel* model)
{

	for (int i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* aianimation = scene->mAnimations[i];

		float lastTime = 0.f;
		Animation animation;
		animation.Duration = aianimation->mDuration;
		animation.TicksPerSecond = aianimation->mTicksPerSecond;
		
		
		for (int j = 0; j < aianimation->mNumChannels; j++)
		{
			Aninode aninode;

			aiNodeAnim* aianinode = aianimation->mChannels[j];
			aninode.name = string(aianinode->mNodeName.C_Str());

			UINT keyCnt = max(aianinode->mNumPositionKeys, aianinode->mNumRotationKeys);
			keyCnt = max(keyCnt, aianinode->mNumScalingKeys);

			//키 프레임 정보 구성
			XMFLOAT3 translation = XMFLOAT3(0.f, 0.f, 0.f);
			XMFLOAT3 scale = XMFLOAT3(1.f, 1.f, 1.f);
			XMFLOAT4 rotation = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
			float	 time = 0.f;

			for (int k = 0; k < keyCnt; k++)
			{

				if (aianinode->mNumPositionKeys > k) {
					auto posKey = aianinode->mPositionKeys[k];
					//memcpy_s(&translation, sizeof(translation), &posKey.mValue, sizeof(posKey.mValue));
					translation = XMFLOAT3(posKey.mValue.x, posKey.mValue.y, posKey.mValue.z);
					time = (float)aianinode->mPositionKeys[k].mTime;
					
				}

				if (aianinode->mNumRotationKeys > k) {
					auto rotKey = aianinode->mRotationKeys[k];
					rotation = XMFLOAT4(rotKey.mValue.x, rotKey.mValue.y, rotKey.mValue.z, rotKey.mValue.w);
					//rotation = XMFLOAT4(rotKey.mValue.w, rotKey.mValue.x, rotKey.mValue.y, rotKey.mValue.z);
					time = (float)aianinode->mRotationKeys[k].mTime;
				}

				if (aianinode->mNumScalingKeys > k) {
					auto scaleKey = aianinode->mScalingKeys[k];
					memcpy_s(&scale, sizeof(scale), &scaleKey.mValue, sizeof(scaleKey.mValue));
					time = (float)aianinode->mScalingKeys[k].mTime;
				}

				aninode.KeyFrames.emplace_back(KeyFrame{ time, translation, rotation, scale });
			}

			lastTime = max(aninode.KeyFrames.back().timePos, lastTime);
			animation.aninodeList.push_back(aninode);
		}

		animation.LastTime = lastTime;

		model->GetAnimationList()->push_back(animation);
	}
}

void ModelLoader::processPhysxGeometry(SkinnedModel* model)
{
	PxBoxGeometry& boxGeo = model->GetBoxGeometry();
	PxReal hx = (Maxpos.x - Minpos.x) / 2;
	PxReal hy = (Maxpos.y - Minpos.y) / 2;
	PxReal hz = (Maxpos.z - Minpos.z) / 2;
	boxGeo = PxBoxGeometry(hx, hy, hz);
	
	model->GetCapsuleGeometry() = PxCapsuleGeometry((Maxpos.x - Minpos.x) / 2, (Maxpos.y - Minpos.y) / 2);

	/*
	PxVec3* vertexdata = new PxVec3[px_vertices.size()];
	std::memcpy(vertexdata, px_vertices.data(), px_vertices.size() * sizeof(PxVec3));
	PxU8* indicedata = new PxU8[px_indices.size()];
	std::memcpy(indicedata, px_indices.data(), px_indices.size() * sizeof(PxU8));

	// PxConvexMeshDesc 생성
	PxConvexMeshDesc convexDesc;
	convexDesc.points.data = vertexdata;
	convexDesc.points.count = px_vertices.size();
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.indices.data = indicedata;
	convexDesc.indices.count = px_indices.size();
	convexDesc.indices.stride = sizeof(PxU8);
	convexDesc.flags = convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	// PxConvexMesh 생성
	PxConvexMesh* convexMesh = PhysicsComponent::GetCooking()->createConvexMesh
					(convexDesc, PhysicsComponent::GetPhysics()->getPhysicsInsertionCallback());
	PxConvexMeshGeometry& convexMeshGeo = model->GetConvexMeshGeometry();
	convexMeshGeo = PxConvexMeshGeometry(convexMesh, PxMeshScale());

	delete[] vertexdata;
	delete[] indicedata;
	*/

	px_vertices.clear();
	px_vertices.shrink_to_fit();
	px_indices.clear();
	px_indices.shrink_to_fit();
	
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
			}
			else {
				std::string filename = std::string(str.C_Str());
				filename = directory_ + '/' + filename;
				std::wstring filenamews = std::wstring(filename.begin(), filename.end());
				hr = CreateWICTextureFromFile(dev_, devcon_, filenamews.c_str(), nullptr, &texture.texture);

				if (FAILED(hr))
				{
					MessageBox(hwnd_, L"Texture couldn't be loaded", L"Error!", MB_ICONERROR | MB_OK);
					continue;
				}
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
}

void ModelLoader::processNode(aiNode * ainode, NodeInfo* node) {
	
	node->name = ainode->mName.C_Str();
	node->transform = XMMatrixTranspose((XMMATRIX(&ainode->mTransformation.a1)));
	//node->transform = XMMATRIX(&ainode->mTransformation.a1);
	for (UINT i = 0; i < ainode->mNumChildren; i++) {
		NodeInfo* childNodeInfo = new NodeInfo;
		this->processNode(ainode->mChildren[i], childNodeInfo);
		node->children.push_back(childNodeInfo);
	}
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


XMMATRIX ModelLoader::ToMat4(aiMatrix4x4 mat)
{
	XMMATRIX nodeTransform(mat.a1, mat.a2, mat.a3, mat.a4,
		mat.b1, mat.b2, mat.b3, mat.b4,
		mat.c1, mat.c2, mat.c3, mat.c4,
		mat.d1, mat.d2, mat.d3, mat.d4);
	return nodeTransform;
}

bool ModelLoader::CreateVertexBuffer(const vector<VERTEX>& vertices, ID3D11Buffer** VertexBuffer)
{
	HRESULT hr;
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT; 
	vbd.ByteWidth = static_cast<UINT>(sizeof(VERTEX) * vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &vertices[0];
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	hr = dev_->CreateBuffer(&vbd, &initData, VertexBuffer);
	if (FAILED(hr)) 
	{
		Close();
		throw std::runtime_error("Failed to create vertex buffer.");
		return false;
	}
	return true;
}

bool ModelLoader::CreateIndexBuffer(const vector<unsigned int>& indices, ID3D11Buffer** IndexBuffer)
{
	HRESULT hr;
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &indices[0];
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;


	hr = dev_->CreateBuffer(&ibd, &initData, IndexBuffer);
	if (FAILED(hr)) 
	{
		Close();
		throw std::runtime_error("Failed to create index buffer.");
	}

	return true;
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
