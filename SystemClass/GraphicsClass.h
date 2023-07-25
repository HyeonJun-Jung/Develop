#pragma once
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include "d3dclass.h"
#include <list>
#include <vector>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class FrustumClass;
class D3DClass;
class CameraClass;
class InputClass;
class TextClass;
class TexturedObject;

class GraphicsClass
{
public:
    GraphicsClass();
    GraphicsClass(const GraphicsClass&);
    ~GraphicsClass();

    bool Initialize(int, int, HWND, InputClass* input);
    bool InitializeModel(int, int, HWND);
    void Shutdown();

    bool Frame(float fDeltaTime);
    bool Update(float fDeltaTime);
    bool Render();

    bool RenderImGui();
    bool RenderScene();

    bool AddBox();

private:
    FrustumClass* m_Frustum = nullptr;
    InputClass* m_Input = nullptr;
    HWND* m_Hwnd = nullptr;

private:
    TextClass* m_Text = nullptr;
    std::vector<TexturedObject*> m_TexturedList;

private:
    float m_fDeltaTime;
};

#endif

