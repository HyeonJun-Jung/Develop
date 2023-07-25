#include "Particle.h"
#include "..\DirectXTex\DirectXTex.h"
#include "AssimpTest/TextureLoader.h"
#include "CameraClass.h"
#include "D3DClass.h"
ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

bool ParticleSystem::Initialize(ID3D11Device* dev)
{
	if (!InitializeShader(dev))
		return false;

	if (!LoadTextures(dev))
		return false;

	return true;
}

bool ParticleSystem::Create(ID3D11Device* dev)
{
	m_Vertices = new VertexBufferType[MAX_VERTEX];

	UpdateParticle();

	if (!InitializeBuffer(dev))
		return false;

	return true;
}

void ParticleSystem::Play(ID3D11DeviceContext* devcon, XMFLOAT3 pos)
{
	UpdateParticle();
	UpdateVertexBuffer(devcon);
}

bool ParticleSystem::Update(ID3D11DeviceContext* devcon, float fDeltatime)
{
	m_SparkType.time += fDeltatime;

	if (m_SparkType.time > m_SparkType.duration)
		Play(devcon, XMFLOAT3(0,0,0));

	UpdateMatrixBuffer(devcon);
	UpdateSparkBuffer(devcon);
	UpdateColorBuffer(devcon);

	return true;
}

bool ParticleSystem::Render(ID3D11DeviceContext* devcon)
{
	D3DClass::GetSingleton()->DisableDepthMask();
	D3DClass::GetSingleton()->EnableParticleBlending();
	
	devcon->VSSetShader(m_FireVS, nullptr, 0);
	devcon->PSSetShader(m_FirePS, nullptr, 0);
	devcon->GSSetShader(m_FireGS, nullptr, 0);

	devcon->VSSetConstantBuffers(0, 1, &m_MatrixBuffer);
	devcon->VSSetConstantBuffers(1, 1, &m_SparkBuffer);
	devcon->GSSetConstantBuffers(0, 1, &m_MatrixBuffer);
	devcon->PSSetConstantBuffers(0, 1, &m_ColorBuffer);

	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	devcon->IASetInputLayout(m_FireLayout);

	UINT stride = sizeof(VertexBufferType);	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	devcon->PSSetShaderResources(0, 1, &m_Textures[m_TextureIndex]);
	devcon->PSSetSamplers(0, 1, &m_SamplerState);
	devcon->Draw(m_ParticleInfo.count, 0);

	D3DClass::GetSingleton()->EnableDepthMask();
	D3DClass::GetSingleton()->TurnOffAlphaBlending();
	devcon->GSSetShader(nullptr, NULL, 0);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return true;
}


bool ParticleSystem::PostRender(ID3D11DeviceContext* devcon)
{
	////////////////////////////////////////////
	// Buttons //
	////////////////////////////////////////////
	// Buttons //
	ImGui::Begin("[ParticleEdior]");
	///////////
	if (ImGui::Button("Create"))
	{

	}
	if (ImGui::Button("Play"))
	{

	}
	
	//
	///////////
	ImGui::End();
	///////////////////////////////
	// Particle Info //
	ImGui::Begin("[ParticleInfo]");
	///////////
	{
		ImGui::InputFloat("xf", &m_xf);
		ImGui::Text("Loop");
		ImGui::Checkbox("LP", &m_ParticleInfo.loop);
		//
		//
		ImVec2 imgSize{ 64.0f, 64.0 };
		ImGui::Text("Texture");
		if (m_Textures[m_TextureIndex])
			ImGui::Image(m_Textures[m_TextureIndex], imgSize);
		//
		ImGui::ListBox("Images", &m_TextureIndex, items.data(), items.size());

		ImGui::Text("Count");
		ImGui::InputInt("CT", (int*)&m_ParticleInfo.count);
		ImGui::Text("Duration");
		ImGui::InputFloat("DR", &m_ParticleInfo.duration);
	}
	{
		ImGui::Text("StartLifeTime");
		ImGui::InputFloat2("lt", (float*)&m_ParticleInfo.startLifeTime);

		ImGui::Text("StartSpeed");
		ImGui::InputFloat2("sp", (float*)&m_ParticleInfo.startSpeed);

		ImGui::Text("StartRadius");
		ImGui::InputFloat2("rd", (float*)&m_ParticleInfo.startRadius);
		float min = m_ParticleInfo.startRadius.x * m_ParticleInfo.startSpeed.x;
		float max = m_ParticleInfo.startRadius.y * m_ParticleInfo.startSpeed.y;

		ImGui::Text("StartSize");
		ImGui::InputFloat2("sz", (float*)&m_ParticleInfo.startSize);
	}
	ImGui::Text("StartColor");
	ImGui::ColorEdit4("cr1", (float*)&m_ParticleInfo.startColor[0]);
	ImGui::ColorEdit4("cr2", (float*)&m_ParticleInfo.startColor[1]);
	{
		ImGui::Text("ColorOverLifeTime");
		ImGui::ColorEdit4("COLT", (float*)&m_ParticleInfo.colorOverLifeTime);

		ImGui::Text("SizeOverLifeTime");
		ImGui::InputFloat("SOLT", &m_ParticleInfo.sizeOverLifeTime);

		ImGui::Text("RotOverLifeTime");
		ImGui::InputFloat3("ROLT", (float*)&m_ParticleInfo.rotOverLifeTime);
	}
	///////////
	ImGui::End();
	return true;
}

void ParticleSystem::UpdateParticle()
{
	m_SparkType.time = 0;
	m_SparkType.duration = m_ParticleInfo.duration;

	for (int i = 0; i < m_ParticleInfo.count; i++)
	{
		{ // LifeTime
			float min = m_ParticleInfo.startLifeTime.x;
			float max = m_ParticleInfo.startLifeTime.y;
			float value = Random(min, max);
			m_Vertices[i].startLifeTime = value;
		}
		{ // Speed
			float min = m_ParticleInfo.startSpeed.x;
			float max = m_ParticleInfo.startSpeed.y;
			float value = Random(min, max);
			m_Vertices[i].speed = value;
			//sparkBuffer->data.startSpeed = value;
		}
		{ // Size
			float min = m_ParticleInfo.startSize.x;
			float max = m_ParticleInfo.startSize.y;
			float value = Random(min, max);
			m_Vertices[i].size = XMFLOAT2(value, value);
		}
		{ // Direction
			float min = m_ParticleInfo.startRadius.x;
			float max = m_ParticleInfo.startRadius.y;
			float radius = Random(min, max);

			float r1 = Random(-1.0f, 1.0f);
			float r2 = Random(-1.0f, 1.0f);
			XMFLOAT3 forward = XMFLOAT3(0, 0, 1); XMVECTOR vecforward = (XMLoadFloat3(&forward) * r1);
			XMFLOAT3 right = XMFLOAT3(1, 0, 0); XMVECTOR vecright = (XMLoadFloat3(&right) * r2);
			XMVECTOR dir = XMVector3Normalize(vecforward + vecright) * radius;
			XMStoreFloat3(&m_Vertices[i].direction, dir);
		}
		// Color
		XMFLOAT4 color0 = m_ParticleInfo.startColor[0];
		XMFLOAT4 color1 = m_ParticleInfo.startColor[1];
		XMFLOAT4 color = {
			(color1.x - color0.x) * Random(0.0f, 1.0f) + color0.x,
			(color1.y - color0.y) * Random(0.0f, 1.0f) + color0.y,
			(color1.z - color0.z) * Random(0.0f, 1.0f) + color0.z,
			1.0f
			//(color1.w - color0.w) * Random(0.0f, 1.0f) + color0.w
		};
		//colorBuffer->data.startColor = color;
		m_Vertices[i].color = color;
	}
	for (int i = 0; i < m_ParticleInfo.count; i++)
	{
		m_Vertices[i].direction;
	}
}

bool ParticleSystem::InitializeShader(ID3D11Device* dev)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	HWND hwnd = GetConsoleWindow();

	WCHAR gsFilename[] = L"../SystemClass/Fire_gs.hlsl";
	ID3D10Blob* gsBuffer = nullptr;

	result = D3DCompileFromFile(gsFilename, NULL, NULL, "FireGeometryShader", "gs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &gsBuffer, &errorMessage);
	if (FAILED(result))
	{
		return false;
	}
	result = dev->CreateGeometryShader(gsBuffer->GetBufferPointer(), gsBuffer->GetBufferSize(), NULL, &m_FireGS);
	if (FAILED(result))
	{
		return false;
	}

	WCHAR vsFilename[] = L"../SystemClass/Fire_vs.hlsl";
	ID3D10Blob* vertexShaderBuffer = nullptr;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "FireVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		return false;
	}
	result = dev->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_FireVS);
	if (FAILED(result))
	{
		return false;
	}

	WCHAR psFilename[] = L"../SystemClass/Fire_ps.hlsl";
	ID3D10Blob* pixelShaderBuffer = nullptr;

	result = D3DCompileFromFile(psFilename, NULL, NULL, "FirePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		return false;
	}

	result = dev->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_FirePS);
	if (FAILED(result))
	{
		return false;
	}

	ID3D11ShaderReflection* reflection;
	D3DReflect(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&reflection);

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	vector<D3D11_INPUT_ELEMENT_DESC> inputLayouts;

	for (UINT i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflection->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask < 4)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask < 8)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask < 16)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		string temp = paramDesc.SemanticName;
		if (temp == "Position")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		int n = temp.find_first_of('_');
		temp = temp.substr(0, n);

		if (temp == "Instance")
		{
			elementDesc.InputSlot = 1;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;
		}

		inputLayouts.emplace_back(elementDesc);
	}

	if(FAILED(dev->CreateInputLayout(inputLayouts.data(), (UINT)inputLayouts.size(),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_FireLayout)))
		return false;

	reflection->Release();

	// 텍스처 샘플러 상태 구조체를 생성 및 설정합니다.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처 샘플러 상태를 만듭니다.
	result = dev->CreateSamplerState(&samplerDesc, &m_SamplerState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool ParticleSystem::InitializeBuffer(ID3D11Device* dev)
{
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = static_cast<UINT>(sizeof(VertexBufferType) * MAX_VERTEX);
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_Vertices;

	HRESULT result = dev->CreateBuffer(&BufferDesc, &initData, &m_VertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	result = dev->CreateBuffer(&BufferDesc, &initData, &m_StreamBuffer);
	if (FAILED(result))
	{
		return false;
	}

	BufferDesc.ByteWidth = sizeof(ParticleMatrix);
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	result = dev->CreateBuffer(&BufferDesc, NULL, &m_MatrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	BufferDesc.ByteWidth = sizeof(SparkBufferType);
	result = dev->CreateBuffer(&BufferDesc, NULL, &m_SparkBuffer);
	if (FAILED(result))
	{
		return false;
	}

	BufferDesc.ByteWidth = sizeof(ColorBufferType);
	result = dev->CreateBuffer(&BufferDesc, NULL, &m_ColorBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool ParticleSystem::LoadTextures(ID3D11Device* dev)
{
	wstring filename = L"../SystemClass/Textures/image1.png";
	ScratchImage image;
	if(FAILED(LoadFromWICFile(filename.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, image)))
		return false;
	if(FAILED(CreateShaderResourceView(dev, image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), &m_Textures[0])))
		return false;

	filename = L"../SystemClass/Textures/image2.png";
	LoadFromWICFile(filename.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, image);
	if (FAILED(CreateShaderResourceView(dev, image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), &m_Textures[1])))
		return false;

	filename = L"../SystemClass/Textures/image3.png";
	LoadFromWICFile(filename.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, image);
	if (FAILED(CreateShaderResourceView(dev, image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), &m_Textures[2])))
		return false;

	filename = L"../SystemClass/Textures/image4.png";
	LoadFromWICFile(filename.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, image);
	if (FAILED(CreateShaderResourceView(dev, image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), &m_Textures[3])))
		return false;

	filename = L"../SystemClass/Textures/image5.png";
	LoadFromWICFile(filename.c_str(), WIC_FLAGS_FORCE_RGB, nullptr, image);
	if (FAILED(CreateShaderResourceView(dev, image.GetImages(), image.GetImageCount(),
		image.GetMetadata(), &m_Textures[4])))
		return false;

	return true;
}

bool ParticleSystem::UpdateMatrixBuffer(ID3D11DeviceContext* devcon)
{
	XMMATRIX viewMatrix; CameraClass::GetSingleton()->GetViewMatrix(viewMatrix);
	m_MatrixType.view = XMMatrixTranspose(viewMatrix);

	XMMATRIX inverseView = XMMatrixInverse(nullptr, viewMatrix);
	m_MatrixType.inverseView = XMMatrixTranspose(inverseView);

	XMMATRIX WorldMatrix = XMMatrixIdentity();
	m_MatrixType.world = XMMatrixTranspose(WorldMatrix);

	XMMATRIX projectionMatrix; D3DClass::GetSingleton()->GetProjectionMatrix(projectionMatrix);
	m_MatrixType.projection = XMMatrixTranspose(projectionMatrix);


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if(FAILED(devcon->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;
	memcpy(mappedResource.pData, &m_MatrixType, sizeof(ParticleMatrix));
	devcon->Unmap(m_MatrixBuffer, 0);

	return true;
}

bool ParticleSystem::UpdateSparkBuffer(ID3D11DeviceContext* devcon)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_SparkBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;
	memcpy(mappedResource.pData, &m_SparkType, sizeof(SparkBufferType));
	devcon->Unmap(m_SparkBuffer, 0);
	return true;
}

bool ParticleSystem::UpdateColorBuffer(ID3D11DeviceContext* devcon)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(devcon->Map(m_ColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;
	memcpy(mappedResource.pData, &m_ColorType, sizeof(ColorBufferType));
	devcon->Unmap(m_ColorBuffer, 0);
	return true;
}


bool ParticleSystem::UpdateVertexBuffer(ID3D11DeviceContext* devcon)
{
	UINT stride = sizeof(VertexBufferType);
	devcon->UpdateSubresource(m_VertexBuffer, 0, nullptr, m_Vertices, stride, m_ParticleInfo.count);
	return true;
}

