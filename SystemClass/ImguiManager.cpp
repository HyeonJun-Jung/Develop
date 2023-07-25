#include "ImguiManager.h"
#include <Windows.h>
#include <iostream>
#include "ModelStructure.h"
ImguiManager* ImguiManager::m_Instance = nullptr;

ImguiManager::ImguiManager()
{
}

ImguiManager::~ImguiManager()
{
    ImGui::DestroyContext();
}

bool ImguiManager::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devicecontext)
{
    this->hwnd = hwnd;
    this->device = device;
    this->devcon = devicecontext;

    
    // Imgui 생성
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, devicecontext);
    ImGui::StyleColorsDark();

    return true;
    
}

void ImguiManager::Update()
{

}

void ImguiManager::BeginFrame()
{
    // start the Dear ImGui Frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static int selected_item = NULL;
    int current_item = 0;
    const char* items[3] = { "File", "Open File", "Save File" };
    bool Open_File = false;


    // Combo의 크기 설정
    ImGui::PushItemWidth(ImGui::GetWindowWidth() / 3);
    if (ImGui::BeginCombo("##custom combo", items[current_item]))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            bool is_selected = (current_item == n);
            if (ImGui::Selectable(items[n], is_selected))
            {
                if (items[n] == items[1])
                    Open_File = true;
                current_item = n;
            }

            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    if (Open_File)
    {
        // 색상이 변경된 후 처음 실행될 때만 실행되는 코드
        OPENFILENAME ofn;
        WCHAR szFile[MAX_PATH] = L"";

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn) == TRUE)
        {
            WCHAR* FileDir = szFile;
            int i = 1;
            i = 2;
        }
    }

    const char* colors[] = { "Red", "Green", "Blue" };
    int current_color = 0;
    bool color_changed = false;

    if (ImGui::BeginCombo("Color", colors[current_color]))
    {
        for (int i = 0; i < IM_ARRAYSIZE(colors); i++)
        {
            bool is_selected = (current_color == i);
            if (ImGui::Selectable(colors[i], is_selected))
            {
                current_color = i;
                color_changed = true;
            }
        }
        ImGui::EndCombo();
    }

    if (color_changed)
    {       
        color_changed = false;
    }
    
    ImGui::PushItemWidth(ImGui::GetWindowWidth() / 3);
    ImGui::Combo("##ItemsCombo", &selected_item, items, IM_ARRAYSIZE(items));
    ImGui::PopItemWidth();
    




    float Scale[3] = { 1.0f, 1.0f, 1.0f };
    ImGui::SliderFloat3("Scale", Scale, 0, 100, "%.3f Scale");


    


   
    ImGui::Begin("Titel");
    
    if (ImGui::Button("Popup"))
        ImGui::OpenPopup("testMenu");

    if (ImGui::BeginPopupModal("testMenu", NULL, ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::BeginMenuBar())
        {
                if (ImGui::BeginMenu("test1"))
                {
                    ImGui::MenuItem("test1_1");
                    ImGui::MenuItem("test1_2");
                    ImGui::EndMenu();
                }

                ImGui::MenuItem("test2");

                if (ImGui::MenuItem("test3"))
                {
                    ImGui::CloseCurrentPopup();
                }

            ImGui::EndMenuBar();
        }

        ImGui::Button("test");
        ImGui::EndPopup();
    }
    ImGui::End();

    // ImGui::Begin("Use AnyWhere");

    //ImGui::ShowMetricsWindow();               // ImGui 헬프
    //ImGui::ColorEdit4("Color", my_color);     // ImGui 컬러 픽업

    //ShowDemoWindowWidgets();
    


}

void ImguiManager::EndFrame()
{
    ImGui::Render();
    //Render Draw Data
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
