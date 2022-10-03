#pragma once
#include <Ren/Ren.h>
#include <Ren/Utils/FpsCounter.hpp>
#include <Ren/Physics/Physics.h>
#include "DemoLayer.hpp"

class ImGuiLayer : public Ren::Layer
{
    Ren::PixelCamera m_pixCam;

public:
    Ref<DemoLayer> m_DemoLayer{ nullptr };
    
    ImGuiLayer(const std::string& name) : Ren::Layer(name) {}

    void OnInit() override
    {
        m_pixCam.SetViewportSize(m_GameCore->GetWindowSize());
    }
    void OnUpdate(float dt) override 
    {
        m_fpsCounter.Update(dt);		
        if (KeyPressed(Ren::Key::I))
            m_showDemo = !m_showDemo;
    }
    void OnEvent(Ren::Event& e) override
    {
        if (e.sdl_event.type == SDL_MOUSEBUTTONDOWN)
            e.handled = true;
    }
    void OnImGui(Ren::ImGuiContext& context) override
    {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N")) {}
                if (ImGui::MenuItem("Open..", "Ctrl+O")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl+S")) {}
                if (ImGui::MenuItem("Save As..")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Quit", "Alt+f4"))
                    m_GameCore->m_Run = false;

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl+U")) {}
                if (ImGui::MenuItem("Redo", "Ctrl+R")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Find", "Ctrl+F")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Preferences", "Ctrl+P")) {}
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
        // m_fpsCounter.DrawPlot(context, m_GameCore->GetWindowSize());
        if (m_showDemo)
            ImGui::ShowDemoWindow();
        
        ImGui::Begin("Scene view");
        glm::vec2 size = m_DemoLayer->m_renderTexture.m_Size;
        ImGui::Image(m_DemoLayer->m_renderTexture.m_Texture, { size.x, size.y });
        ImGui::End();

        ImGui::Begin("Info");
        ImGui::End();

        ImGui::Begin("Scene");
        ImGui::End();

        ImGui::Begin("Logger");
        ImGui::End();

        ImGui::Begin("Options");
        ImGui::End();
    }
private:
    bool m_showDemo{ false };
    Ren::Utils::FpsCounter m_fpsCounter;
};