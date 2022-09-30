#pragma once
#include <Ren/Ren.h>
#include <Ren/Utils/FpsCounter.hpp>

class ImGuiLayer : public Ren::Layer
{
public:
    ImGuiLayer(const std::string& name) : Ren::Layer(name) {}

    void OnUpdate(float dt) override 
    {
        m_fpsCounter.Update(dt);		
        if (KeyPressed(Ren::Key::I))
            m_showDemo = !m_showDemo;
    }
    
    void OnEvent(Ren::Event& e)
    {
        if (e.sdl_event.type == SDL_MOUSEBUTTONDOWN)
            e.handled = true;
    }
    void OnImGui(Ren::ImGuiContext& context)
    {
        m_fpsCounter.DrawPlot(context, m_GameCore->GetWindowSize());
        if (m_showDemo)
            ImGui::ShowDemoWindow();
    }
private:
    bool m_showDemo{ false };
    Ren::Utils::FpsCounter m_fpsCounter;
};