#pragma once
#include "..\stdafx.h"
const int MAX_TRIANGLES = 10000;
class TerrainClass;
class FrustumClass;
class TerrainShaderClass;

class QuadtreeClass
{
	struct VertexType
	{
		XMFLOAT3 Position;
		XMFLOAT2 Texcoord;
		XMFLOAT3 Normal;
	};
	
	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer* vertexBuffer; ID3D11Buffer* indexBuffer;
		NodeType* Child[4];
	};

public:
	QuadtreeClass();
	QuadtreeClass(const QuadtreeClass&);
	~QuadtreeClass();
	bool Initialize(TerrainClass* terrain, ID3D11Device* device);
	void Shutdown();
	bool Render(FrustumClass* frustum, TerrainShaderClass* Shader, ID3D11DeviceContext* device);


private:
	void CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& width);
	void CreateQuadtree(ID3D11Device* device, NodeType* Node, float PositionX, float PositionZ, float width);
	int CountTriangles(float positionX, float positionZ, float width);
	bool IsTriangleContained(int index, float positionX, float positionZ, float width);
	void RenderNode(NodeType* node, FrustumClass* frustum, ID3D11DeviceContext* devcon, TerrainShaderClass* shader);
	void ReleaseNode(NodeType* ParentNode);

	int m_triangleCount, m_drawnCount;
	VertexType* m_VertexList = nullptr;
	NodeType* m_RootNode = nullptr;;
};

