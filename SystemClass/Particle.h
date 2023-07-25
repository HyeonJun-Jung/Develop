#pragma once
#include "Object.h"

#define MAX_VERTEX 1000

class ParticleSystem :
    public Object
{
    struct VertexBufferType {
        XMFLOAT3 position = { 0, 0, 0 };
        XMFLOAT2 size = { 1.0f, 1.0f };
        XMFLOAT3 direction;
        float startLifeTime;
        float speed = 1.0f;
        XMFLOAT4 color;
        XMFLOAT2 padding;
    };

    struct SparkBufferType
    {
        float time = 0;
        float duration = 3;
        //
        XMFLOAT2 startSize = { 1, 1 };
        XMFLOAT3 startDirection = {0, 0, 0 };
        //
        float sizeOverLifeTime = 0;
        XMFLOAT3 rotOverLifeTime = { 0, 0 ,0 };
        //
        float padding;
    };

    struct ColorBufferType {
        XMFLOAT4 startColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        XMFLOAT4 colorOverLifeTime = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
    };

    struct ParticleMatrix {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX inverseView;
        XMMATRIX projection;
    };

    struct ParticleInfo {
        int shape{};
        bool loop{ true };
        UINT count{ 100 };
        float duration{ 3.0f };
        //
        XMFLOAT2 startLifeTime{ 1.0f, 3.0f };
        XMFLOAT2 startSpeed{ 1.0f, 3.0f };
        XMFLOAT2 startRadius{ 1.0f, 5.0f };
        XMFLOAT2 startSize{ 1.0f, 3.0f };
        XMFLOAT4 startColor[2]{ {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
        //
        XMFLOAT4 colorOverLifeTime{ 1.0f, 1.0f, 1.0f, 0 };
        float sizeOverLifeTime{ 0.0f };
        XMFLOAT3 rotOverLifeTime{};
    };

public:
    ParticleSystem();
    virtual ~ParticleSystem();

public:
    bool Initialize(ID3D11Device* dev);
    bool Create(ID3D11Device* dev);
    void Play(ID3D11DeviceContext* devcon, XMFLOAT3 pos);
    bool Update(ID3D11DeviceContext* devcon, float fDeltatime);
    bool Render(ID3D11DeviceContext* devcon);
    bool PostRender(ID3D11DeviceContext* devcon);

private:
    bool InitializeShader(ID3D11Device* dev);
    bool InitializeBuffer(ID3D11Device* dev);
    bool UpdateVertexBuffer(ID3D11DeviceContext* devcon);
    bool UpdateSparkBuffer(ID3D11DeviceContext* devcon);
    bool UpdateColorBuffer(ID3D11DeviceContext* devcon);
    bool UpdateMatrixBuffer(ID3D11DeviceContext* devcon);
    bool LoadTextures(ID3D11Device* dev);
    void UpdateParticle();

private:
    float Random(float min, float max) 
    {
        
        float normal = rand() / (float)RAND_MAX;
        return min + (max - min) * normal;
    }

    int Random(int min, int max) { return rand() % (max - min) + min; }

private:
    ID3D11GeometryShader* m_FireGS;
    ID3D11VertexShader* m_FireVS;
    ID3D11PixelShader* m_FirePS;
    ID3D11InputLayout* m_FireLayout;

    ID3D11Buffer* m_StreamBuffer;
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;

    ParticleMatrix m_MatrixType;
    ID3D11Buffer* m_MatrixBuffer;

    SparkBufferType m_SparkType;
    ID3D11Buffer* m_SparkBuffer;

    ID3D11Buffer* m_ColorBuffer;
    ColorBufferType m_ColorType;

    VertexBufferType* m_Vertices;
    ParticleInfo m_ParticleInfo;

     
    ID3D11ShaderResourceView* m_Textures[5];
    ID3D11SamplerState* m_SamplerState;

    int m_TextureIndex = 4;
    float m_xf = 0.6;

     vector<const char*> items{ "image1", "image2", "image3", "image4", "image5" };;

};

