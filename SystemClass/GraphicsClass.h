#pragma once
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "d3dclass.h"
#include <list>

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class D3DClass;
class CameraClass;
class ModelClass;
class LightClass;
class TextClass;
class WModel;
class FrustumClass;

class ModelLoader;

class BumpMapShaderClass;
class ReflectionShaderClass;
class LightShaderClass;
class TextureShaderClass;
class ColorShaderClass;

class RenderTextureClass;

class TerrainClass;
class TerrainShaderClass;
class TerrainReflectionShaderClass;
class SkydomeClass;
class SkydomeShaderClass;
class WaterShaderClass;




////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
    GraphicsClass();
    GraphicsClass(const GraphicsClass&);
    ~GraphicsClass();

    bool Initialize(int, int, HWND);
    bool InitializeShader(HWND);
    bool InitializeModel(int, int, HWND);
    void Shutdown();

    bool Frame(float fDeltaTime);
    bool Update(float fDeltaTime);
    bool Render();

    bool RenderImGui();
    bool RenderScene();
    bool RenderReflectionToTexture();
    bool RenderRefractionToTexture();

private:
    FrustumClass* m_Frustum = nullptr;
    HWND* m_Hwnd = nullptr;

private:
    /*
    list<class DynamicModel*> m_DynamicList;
    list<class StaticModel*> m_StaticList;
    list<class TerrainClass*> m_TerrainList;
    */


private:
    ModelClass* m_FloorModel = nullptr;
    LightClass* m_Light = nullptr;
    TextClass* m_Text = nullptr;
    RenderTextureClass* m_ReflectionRenderTexture = nullptr;
    RenderTextureClass* m_RefractionRenderTexture = nullptr;
    TerrainClass* m_Terrain = nullptr;
    SkydomeClass* m_Skydome = nullptr;
    WModel* m_Wmodel;


private:
    ReflectionShaderClass* m_ReflectionShader = nullptr;
    BumpMapShaderClass* m_BumpMapShader = nullptr;
    TerrainShaderClass* m_TerrainShader = nullptr;
    TerrainReflectionShaderClass* m_TerrainReflectionShader = nullptr;
    SkydomeShaderClass* m_SkydomeShader = nullptr;
    WaterShaderClass* m_WaterShader = nullptr;
    float m_waterHeight = 0;
    float m_waterTranslation = 0;



    float m_fDeltaTime;
};

#endif

