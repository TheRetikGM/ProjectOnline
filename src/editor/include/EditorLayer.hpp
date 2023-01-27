#pragma once
#include <Ren/Ren.h>
#include "GuiLogger.hpp"
#include "imgui.h"
#include "Scene.h"

class EditorLayer : public Ren::Layer
{
    Ren::Utils::FpsCounter m_fpsCounter;
    GuiLogger* m_logger;
    Ref<Scene> m_scene;

public:
    EditorLayer(std::string name) : Ren::Layer(name) { }

    void OnInit() override
    {
        m_logger  = Ren::LogEmmiter::AddListener<GuiLogger>();
        m_scene = CreateRef<Scene>(glm::ivec2( 1200, 700 ), GetRenderer(), GetInput());
        m_scene->Load("param.yaml");
        m_scene->SetDebug(true);
    }

    void OnEvent(Ren::Event& e) override
    {
        // Zoom in and out with mouse wheel
        if (e.event.type == SDL_MOUSEWHEEL && e.event.wheel.y != 0)
            m_scene->ProcessMouseWheel(e.event.wheel.y);
    }

    void OnUpdate(float dt) override
    {
        if (KeyPressed(Ren::Key::ESCAPE))
            m_GameCore->m_Run = false;

        m_scene->ProcessInput(dt);
        m_scene->Update(dt);
        m_fpsCounter.Update(dt);
    }

    void OnImGui(Ren::ImGuiContext& context) override
    {
        ImGui::DockSpaceOverViewport();

        m_logger->Draw();
        
        ImGui::Begin("Info");
        m_fpsCounter.DrawPlot();
        ImGui::End();

        ImGui::Begin("Scene view");
        ImGui::Image((ImU64)m_scene->m_SceneTexture.m_Texture, { (float)m_scene->GetSize().x, (float)m_scene->GetSize().y });
        ImVec2 v_min = ImGui::GetWindowContentRegionMin();
        ImVec2 v_max = ImGui::GetWindowContentRegionMax();
        v_min.x += ImGui::GetWindowPos().x;
        v_min.y += ImGui::GetWindowPos().y;
        v_max.x += ImGui::GetWindowPos().x;
        v_max.y += ImGui::GetWindowPos().y;
        glm::ivec2 content_size = { v_max.x - v_min.x, v_max.y - v_min.y };
        static ImVec2 last_win_size = { 0.0f,  0.0f };
        ImVec2 current_win_size = ImGui::GetWindowSize();
        // m_scene-GetSize() is temporary. It is for the initial size. However that will be set by setting m_scene->Resize() after loading a scene.
        if (last_win_size.x != current_win_size.x || last_win_size.y != current_win_size.y || m_scene->GetSize().x != content_size.x || m_scene->GetSize().y != content_size.y)
                m_scene->Resize(content_size);
        last_win_size = current_win_size;
        m_scene->m_ScreenPos = { v_min.x, v_min.y };
        ImGui::End();

        ImGui::Begin("Scene tree");
        ImGui::End();

        ImGui::Begin("Entity details");
        ImGui::End();

        ImGui::Begin("Settings");
        ImGui::End();
    }

    void OnRender(SDL_Renderer* renderer) override
    {
        m_scene->Render();
    }
};
