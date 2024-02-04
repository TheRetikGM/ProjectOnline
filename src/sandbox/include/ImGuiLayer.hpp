#pragma once
#include <Ren/Ren.h>
#include <Ren/Utils/FpsCounter.hpp>
#include <Ren/Physics/Physics.h>
#include "DemoLayer.hpp"
#include "GuiLogger.hpp"
#define BIT_TEST(a, b) ((a & b) == b)

class ImGuiLayer : public Ren::Layer {
public:
    Ref<DemoLayer> m_DemoLayer{ nullptr };
    GuiLogger* m_logger;

    ImGuiLayer(const std::string& name) : Ren::Layer(name) {
        m_logger = ren_utils::LogEmitter::AddListener<GuiLogger>();
        assert(m_logger != nullptr);
    }

    void OnInit() override {
        m_pixCam.SetViewportSize(m_GameCore->GetWindowSize());
    }
    void OnUpdate(float dt) override {
        m_fpsCounter.Update(dt);
        if (KeyPressed(Ren::Key::I))
            m_showDemo = !m_showDemo;
    }
    void OnEvent(Ren::Event& e) override {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        if (!m_grabInput) {
            e.handled = true;
            return;
        }

        if (BIT_TEST(e.event.type, SDL_MOUSEMOTION))
            ;
    }
    void OnImGui(Ren::ImGuiContext& context) override {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
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
            if (ImGui::BeginMenu("Edit")) {
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
        ImGui::Image((ImU64)m_DemoLayer->m_renderTexture.m_Texture, { size.x, size.y });
        ImGui::End();

        ImGui::Begin("Info");
        m_fpsCounter.DrawPlot();
        ImGui::End();

        ImGui::Begin("Scene");
        ImGui::End();

        ImGui::Begin("Logger");
        m_logger->Draw();
        ImGui::End();

        ImGui::Begin("Options");
        ImGui::Checkbox("Scene grab input", &m_grabInput);
        static bool show_demo = false;
        ImGui::Checkbox("Show demo", &show_demo);
        ImGui::Separator();
        if (ImGui::Button("Exit"))
            m_GameCore->m_Run = false;
        ImGui::End();

        ImGui::Begin("Component");
        ImGui::End();

        if (show_demo)
            ImGui::ShowDemoWindow();
    }
private:
    bool m_showDemo{ false };
    Ren::Utils::FpsCounter m_fpsCounter;
    Ren::PixelCamera m_pixCam;
    bool m_grabInput = false;
};
