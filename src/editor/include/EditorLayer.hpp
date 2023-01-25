#pragma once
#include <Ren/Ren.h>

class EditorLayer : public Ren::Layer
{
    Ref<Ren::Scene> m_scene;
    Ren::CartesianCamera m_camera;
public:
    EditorLayer(std::string name) : Ren::Layer(name) { }

    void OnInit() override
    {
        m_scene = Ren::SceneSerializer::Deserialze("param.yaml", GetRenderer(), GetInput());
        m_scene->GetSystem<Ren::PhysicsSystem>()->m_DebugRender = true;
        m_scene->Init();

        m_camera.SetViewportSize(m_GameCore->GetWindowSize());
        m_camera.SetUnitScale(50);
    }

    void OnUpdate(float dt) override
    {
        if (KeyPressed(Ren::Key::ESCAPE))
            m_GameCore->m_Run = false;

        m_scene->Update(dt);
    }

    void OnImGui(Ren::ImGuiContext& context) override
    {
        ImGui::ShowDemoWindow();
    }

    void OnRender(SDL_Renderer* renderer) override
    {
        Ren::Renderer::BeginRender(&m_camera);
        Ren::Renderer::Clear({ 20, 20, 20, 255 });
        m_scene->Render();
        Ren::Renderer::Render();
        Ren::Renderer::EndRender();
    }
};
