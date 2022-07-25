#pragma once
#include <Ren/Ren.h>
#include <Ren/Core/Camera.h>
#include <yaml-cpp/yaml.h>

#include "scripts/Movement.hpp"

using namespace entt::literals;

struct OutlineComponent
{
	glm::ivec4 color = glm::ivec4(255);
};
class OutlineSystem : public Ren::ComponentSystem
{    glm::vec2 m_rectPos{ 100, 100 };
public:
	OutlineSystem(Ren::Scene* p_scene, Ren::KeyInterface* p_input, SDL_Renderer* renderer) : Ren::ComponentSystem(p_scene, p_input), m_renderer(renderer) {}

	void Render() override
	{
		auto view = m_scene->SceneView<OutlineComponent, Ren::TransformComponent>();
		for (auto&& ent : view)
		{
			auto [outline, trans] = view.get(ent);
			Ren::Renderer::SetRenderLayer(trans.layer);
			Ren::Renderer::DrawRect({ trans.position, trans.scale }, trans.rotation, outline.color);
		}
	}
private:
	SDL_Renderer* m_renderer = nullptr;
};

class DemoLayer : public Ren::Layer
{
	Ref<Ren::TextRenderer> m_textRenderer = Ren::TextRenderer::Create();

	// EnTT //
	// Stores all entities and manages their creation etc.
	Ren::Scene* m_scene;
	// Example entity.
	Ren::Entity m_ent;

	Ren::CartesianCamera m_camera;
public:
    DemoLayer(const std::string& name) : Ren::Layer(name) {}

    void OnInit() override
    {
        // Set background color.
		m_GameCore->m_ClearColor = { 100, 100, 100, 255 };

		m_scene = new Ren::Scene(GetRenderer(), GetInput());
		m_scene->AddSystem<OutlineSystem>(GetRenderer());

		// Create awesomeface entity.
		m_ent = m_scene->CreateEntity({ glm::vec2(1.0f), glm::vec2(2.0f) });
		m_ent.Add<Ren::SpriteComponent>(ASSETS_DIR "awesomeface.png").color = Ren::Colors3::Magenta;
		m_ent.Add<OutlineComponent>().color = Ren::Colors4::Cyan;
		m_ent.Add<Ren::NativeScriptComponent>().Bind<MovementScript>();
		m_ent.Get<Ren::TagComponent>().tag = "awesomeface";
		m_ent.Get<Ren::TransformComponent>().layer = 2;

		// Create copy of awesomeface entity (the texture will get reused).
		auto ent_copy = m_scene->CreateEntity({ glm::vec2(0.0f), glm::vec2(2.0f) });
		ent_copy.Add<Ren::SpriteComponent>(ASSETS_DIR "awesomeface.png");
		ent_copy.Get<Ren::TagComponent>().tag = "rotate";
		ent_copy.Get<Ren::TransformComponent>().layer = 1;

		// Call init on all scene subsystems.
		m_scene->Init();

		m_textRenderer->Load(ASSETS_DIR "fonts/DejaVuSansCondensed.ttf", 32);

		m_camera.SetUnitScale(100);
		m_camera.SetViewportSize(m_GameCore->GetWindowSize());
    }
    void OnDestroy() override
    {
        // Destroy native script component.
		m_scene->Destroy();

		delete m_scene;
    }
    void OnEvent(Ren::Event& e) override
    {
        if (!(m_GameCore->m_Run = !Ren::Utils::key_pressed(e.sdl_event, SDLK_ESCAPE)))
            e.handled = true;
		if (e.sdl_event.type == SDL_MOUSEWHEEL)
		{
			if (e.sdl_event.wheel.y > 0)	// scroll up
			{
				m_camera.SetUnitScale(m_camera.GetUnitScale() + glm::ivec2(10));
			}
			else
			{
				m_camera.SetUnitScale(m_camera.GetUnitScale() - glm::ivec2(10));
			}
		}
		
		if (Ren::Utils::key_pressed(e.sdl_event, SDLK_SPACE))
			m_camera.SetUnitScale(100);
    }
    void OnUpdate(float dt) override
    {
        // Rotate entities with 'rotate' tag.
		const float rotation_speed = 90.0f;		// 90 degrees per second.
		auto entities = m_scene->GetEntitiesByTag("rotate");
		for (auto&& ent : *entities)
			ent.Get<Ren::TransformComponent>().rotation += rotation_speed * dt;

		if (KeyPressed(Ren::Key::SPACE))
			m_camera.m_CamPos = glm::vec2(0.0f);

		float move_speed = 8.0f;
		if (KeyHeld(Ren::Key::LEFT))
			m_camera.m_CamPos -= Ren::RightDir() * move_speed * dt;
		if (KeyHeld(Ren::Key::RIGHT))
			m_camera.m_CamPos += Ren::RightDir() * move_speed * dt;
		if (KeyHeld(Ren::Key::UP))
			m_camera.m_CamPos += Ren::UpDir() * move_speed * dt;
		if (KeyHeld(Ren::Key::DOWN))
			m_camera.m_CamPos -= Ren::UpDir() * move_speed * dt;

		glm::ivec2 mouse_rel_pos(0);
		bool mouse_pressed = SDL_GetRelativeMouseState(&mouse_rel_pos.x, &mouse_rel_pos.y) & SDL_BUTTON_RMASK;
		if (mouse_pressed)
			m_camera.m_CamPos -= glm::vec2(mouse_rel_pos.x, -mouse_rel_pos.y) / glm::vec2(m_camera.GetUnitScale());

		m_scene->Update(dt);
    }
    void OnRender(SDL_Renderer* renderer) override
    {
		Ren::Renderer::BeginRender(&m_camera);
		m_textRenderer->RenderText("WSAD for movement\nArrow keys for camera movement\n'i' to toggle imgui demo window\nESC to exit", { 10.0f, 10.0f }, 1.0f, Ren::Colors3::White, 10);
        m_scene->Render();
		Ren::Renderer::DrawCircle(glm::vec2(0.0f), 0.01f, Ren::Colors4::White, 5);
		Ren::Renderer::DrawCircle({ 0.0f, 0.0f }, 0.5f, Ren::Colors4::Red);
		Ren::Renderer::Render();
    }
    void OnImGui(Ren::ImGuiContext& context) override
    {
        static bool show_demo = false;
		if (KeyPressed(Ren::Key::I))
			show_demo = !show_demo;
		
		if (show_demo)
			ImGui::ShowDemoWindow();
    }
};