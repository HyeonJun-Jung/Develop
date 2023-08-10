#include "Text.h"
#include "FontClass.h"
#include "FontShaderClass.h"

Text::Text()
{
}

bool Text::Initialize(ID3D11Device* dev, ID3D11DeviceContext* devcon)
{
    m_Font = new FontClass;
    if (!m_Font) return false;

    char FontTxtFilename[] = "../SystemClass/data/fontdata.txt";
    WCHAR FontddsFilename[] = L"../SystemClass/data/font.dds";
    if (m_Font->Initialize(dev, FontTxtFilename, FontddsFilename))
        return false;

    m_TextShader = new FontShaderClass;
    if (!m_TextShader) return false;

   /* 
    if (!m_TextShader->Initialize(device, hwnd))
        return false;
    */

    return true;
}


bool Text::Update(ID3D11DeviceContext* devcon, float fDeltaTime)
{
    return true;
}

bool Text::Render(ID3D11DeviceContext* devcon)
{
    return true;
}

bool Text::PostRender(ID3D11DeviceContext* devcon)
{
    return true;
}

bool Text::AddText(ID3D11Device* dev, char* text, float positionX, float positionY)
{
    if (!m_Font) return false;

    // m_Font->BuildVertexArray()
    return true;
}

bool Text::AddTextWithValue(ID3D11Device* dev, char* text, int* value, float positionX, float positionY)
{
    return true;
}

bool Text::AddTextWithValue(ID3D11Device* dev, char* text, float* value, float positionX, float positionY)
{
    return true;
}

bool Text::InitializeText(ID3D11Device* dev, char* text, float positionX, float positionY)
{
    return true;
}

bool Text::InitializeTextWithValue(ID3D11Device* dev, char* text, int* value, float positionX, float positionY)
{
    return true;
}

bool Text::InitializeTextWithValue(ID3D11Device* dev, char* text, float* value, float positionX, float positionY)
{
    return true;
}
