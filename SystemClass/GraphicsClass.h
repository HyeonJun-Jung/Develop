#pragma once
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "d3dclass.h"


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
class LightShaderClass;

class TextureShaderClass;
class BitmapClass;
class TextClass;

class FrustumClass;
class ModelListClass;

class ModelLoader;
class BumpMapShaderClass;

class ReflectionShaderClass;
class RenderTextureClass;



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
    void Shutdown();
    bool Frame(float fDeltaTime);
    bool Render();
    bool RenderScene();
    bool RenderToTexture();

    CameraClass* GetCamera();

private:
    D3DClass* m_Direct3D = nullptr;
    HWND* m_Hwnd = nullptr;
    CameraClass* m_Camera = nullptr;
    ModelClass* m_Model = nullptr;
    LightShaderClass* m_LightShader = nullptr;
    BumpMapShaderClass* m_BumpMapShader = nullptr;
    LightClass* m_Light = nullptr;

    BitmapClass* m_Bitmap = nullptr;
    TextureShaderClass* m_TextureShader = nullptr;

    TextClass* m_Text = nullptr;

    FrustumClass* m_Frustum = nullptr;
    ModelListClass* m_ModelList = nullptr;

    ModelLoader* m_ModelLoader = nullptr;
    ModelClass* m_FloorModel = nullptr;

    ReflectionShaderClass* m_ReflectionShader = nullptr;
    RenderTextureClass* m_RenderTexture = nullptr;

    float m_fDeltaTime;
};

#endif

