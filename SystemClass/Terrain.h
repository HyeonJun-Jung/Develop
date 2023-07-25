#pragma once
#include "Object.h"

class TerrainGraphicC;
class StaticPhysicsC;
const int TEXTURE_REPEAT = 8;
const int MAX_TRIANGLES = 10000;

struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texcoords;
	XMFLOAT3 normal;
};

class Terrain : public Object
{
public:
	Terrain();
	virtual ~Terrain();

private:
	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

	// Quadtree Node
	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		VectorType* vertexArray;
		NodeType* nodes[4];
	};

public:
	bool Initialize(ID3D11Device* device, const char* HeightMapFileName, const WCHAR* TextureFilename);
	void shutdown();

	virtual bool UpdateInput(InputClass* input, float fDeltaTime);
	virtual bool Render(ID3D11DeviceContext* devcon);
	virtual bool PostRender(ID3D11DeviceContext* devcon);

	int GetVertexCount() { return m_VertexCount; }
	int GetIndexCount() { return m_IndexCount; }
	ID3D11Buffer* GetVertexBuffer() { return m_VertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_IndexBuffer; }
	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadHeightMap(const char*);
	void NormalizeHeightMap();
	void ShutdownHeightMap();
	bool CalculateNormals();
	void CalculateTextureCoord();

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

	bool InitializeComponent(ID3D11Device* dev);

	// QuadTree
	bool GetHeightAtPosition(float, float, float&);

	// QuadTree
private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(NodeType*, float, float, float, ID3D11Device*);
	int CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ReleaseNode(NodeType*);
	void FindNode(NodeType*, float, float, float&);
	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

private:
	int m_terrainWidth = 0;
	int m_terrainHeight = 0;
	int m_VertexCount = 0;
	int m_IndexCount = 0;
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;
	HeightMapType* m_heightMap = nullptr;
	ID3D11ShaderResourceView* m_Texture = nullptr;
	VertexType* m_Vertices = nullptr;

	// QuadTree
private:
	int m_triangleCount, m_drawCount;
	NodeType* m_parentNode = nullptr;

private:
	TerrainGraphicC* m_GraphicC;
	StaticPhysicsC* m_PhysicsC;
	PxHeightFieldGeometry m_HeightFieldGeometry;



private:
	struct InputDesc
	{
		UINT index;
		XMFLOAT3 v0, v1, v2;
	};

	struct OutputDesc
	{
		int picked;
		float u, v, distance;
	};

	struct Brush
	{
		int type;
		XMFLOAT3 location;
		float range;
		XMFLOAT3 color;
	};

	struct RayBufferType {
		XMFLOAT3 position;
		float size;
		XMFLOAT3 Direction;
		float padding;
	};

private:
	bool InitializeComputeShader(ID3D11Device* device);
	bool ComputePicking(ID3D11DeviceContext* devcon ,XMFLOAT3* position);
	void UpdateTerrainHeight(XMFLOAT3 position, float fDeltaTime);
	bool UpdateVertexBuffer(ID3D11DeviceContext* devcon);

private:
	ID3D11ComputeShader* m_ComputeShader = nullptr;
	ID3D11Resource* m_ComputeInput = nullptr;
	ID3D11ShaderResourceView* m_ComputeSRV = nullptr;
	ID3D11Resource* m_ComputeOutput = nullptr;
	ID3D11UnorderedAccessView* m_ComputeUAV = nullptr;
	ID3D11Resource* m_Result = nullptr;

	bool m_IsDraw = true;
	bool m_IsRaise = true;
	float m_AdjustValue = 50.f;
	Brush m_Brush;
	float m_Brushlocation[3];
	float m_MousePosition[2];
	float m_MouseWorldPosition[3];
	ID3D11Buffer* m_BrushBuffer = nullptr;

	ID3D11Buffer* m_RayBuffer = nullptr;

	InputDesc* m_input;
	OutputDesc* m_output;

	InputClass* m_InputClass;
};

