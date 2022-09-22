#pragma once
#include <Ren/Ren.h>
#include <box2d/box2d.h>
#include <fstream>

#include "scripts/Movement.hpp"

using namespace entt::literals;

class DemoLayer : public Ren::Layer
{
	Ref<Ren::TextRenderer> m_textRenderer = Ren::TextRenderer::Create();
	Ref<Ren::Scene> m_scene;
	Ren::Entity m_ent;
	Ren::CartesianCamera m_camera;
public:
    DemoLayer(const std::string& name) : Ren::Layer(name) 
	{
		// Outputs logs to stdout by default.
		Ren::LogEmmiter::AddListener<Ren::StreamLogger>();
	}

    void OnInit() override
    {
        // Set background color.
		m_GameCore->m_ClearColor = { 100, 100, 100, 255 };

		m_scene = Ren::SceneSerializer::Deserialze(ASSETS_DIR "scenes/demo.ren", GetRenderer(), GetInput());
		m_scene->GetSystem<Ren::PhysicsSystem>()->m_DebugRender = true;
		auto [success, ent] = m_scene->GetEntityByTag("awesomeface");
		if (success)
			ent.Add<Ren::NativeScriptComponent>().Bind<MovementScript>();
		m_scene->Init();

		m_textRenderer->Load(ASSETS_DIR "fonts/DejaVuSansCondensed.ttf", 32);
		m_camera.SetUnitScale(50);
		m_camera.SetViewportSize(m_GameCore->GetWindowSize());
    }
    void OnDestroy() override
    {
        // Destroy native script component.
		m_scene->Destroy();
		m_scene.reset();
    }
    void OnEvent(Ren::Event& e) override
    {
		if (e.sdl_event.type == SDL_WINDOWEVENT && e.sdl_event.window.event == SDL_WINDOWEVENT_RESIZED)
			m_camera.SetViewportSize(m_GameCore->GetWindowSize());

        if (!(m_GameCore->m_Run = !Ren::Utils::key_pressed(e.sdl_event, SDLK_ESCAPE)))
            e.handled = true;
		if (e.sdl_event.type == SDL_MOUSEWHEEL) 
		{
			if (e.sdl_event.wheel.y > 0)	// scroll up
				m_camera.SetUnitScale(m_camera.GetUnitScale() + glm::ivec2(10));
			else
				m_camera.SetUnitScale(m_camera.GetUnitScale() - glm::ivec2(10));
		}
		if (e.sdl_event.type == SDL_MOUSEBUTTONDOWN && e.sdl_event.button.button == SDL_BUTTON_LEFT)
		{
			glm::ivec2 mouse_pos(0);
			SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
			glm::vec2 pos = m_camera.ToUnits(glm::vec2(mouse_pos));

			Ren::Entity dynamic_body = m_scene->CreateEntity({{ pos.x, pos.y }}, { "spawned_body" });
			auto& dynamic_body_r = dynamic_body.Add<Ren::RigidBodyComponent>();
			auto box_shape = CreateRef<b2PolygonShape>();
			box_shape->SetAsBox(Ren::Utils::RandomFloat(0.5f, 2.0f), Ren::Utils::RandomFloat(0.5f, 2.0f));
			dynamic_body_r.p_shape = box_shape;
			dynamic_body_r.body_def.type = b2_dynamicBody;
			dynamic_body_r.fixture_def.density = Ren::Utils::RandomFloat(0.5f, 5.0f);
			dynamic_body_r.fixture_def.friction = Ren::Utils::RandomFloat_0_1();
			dynamic_body_r.fixture_def.restitution = Ren::Utils::RandomFloat_0_1();

			m_scene->InitPhysicsBody(dynamic_body);
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
		m_textRenderer->RenderText(
			"WSAD for movement\n"
			"Arrow keys or hold mouse right button for camera movement\n"
			"'i' to toggle imgui demo window\n"
			"ESC to exit\n"
			"Mouse left button for spawning new body\n"
			"Mouse wheel for zoom"
			, { 10.0f, 10.0f }, 1.0f, Ren::Colors3::White, 10);
        m_scene->Render();
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