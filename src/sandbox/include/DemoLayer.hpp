#pragma once
#include <Ren/Ren.h>
#include <box2d/box2d.h>
#include <fstream>

#include "scripts/Movement.hpp"

using namespace entt::literals;

class DemoLayer : public Ren::Layer
{
	Ref<Ren::TextRenderer> m_textRenderer = Ren::TextRenderer::Create();
	Ren::Scene* m_scene;
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

		m_scene = new Ren::Scene(GetRenderer(), GetInput());
		m_scene->AddSystem<Ren::PhysicsSystem>()->m_DebugRender = true;

		// Create awesomeface entity.
		m_ent = m_scene->CreateEntity({ glm::vec2(1.0f), glm::vec2(2.0f) }, { "awesomeface" });
		m_ent.Add<Ren::SpriteComponent>(ASSETS_DIR "awesomeface.png").m_Color = Ren::Colors3::Magenta;
		m_ent.Add<Ren::NativeScriptComponent>().Bind<MovementScript>();
		m_ent.Get<Ren::TransformComponent>().layer = 2;
		auto& rig = m_ent.Add<Ren::RigidBodyComponent>();
		auto size = m_ent.Get<Ren::SpriteComponent>().GetSize() * 0.5f;
		auto shape = new b2CircleShape();
		shape->m_radius = size.x;
		rig.body_def.type = b2_dynamicBody;
		rig.p_shape = shape;
		rig.fixture_def.density = 1.0f;
		rig.fixture_def.friction = 0.3f;
		rig.fixture_def.restitution = 0.67;


		// Create copy of awesomeface entity (the texture will get reused).
		auto ent_copy = m_scene->CreateEntity({ glm::vec2(0.0f), glm::vec2(2.0f) }, { "rotate" });
		ent_copy.Add<Ren::SpriteComponent>(ASSETS_DIR "awesomeface.png");
		ent_copy.Get<Ren::TransformComponent>().layer = 1;


		m_textRenderer->Load(ASSETS_DIR "fonts/DejaVuSansCondensed.ttf", 32);

		m_camera.SetUnitScale(50);
		m_camera.SetViewportSize(m_GameCore->GetWindowSize());

		/////////// Box2D ///////////
		{
			auto box_shape = new b2PolygonShape();

			Ren::Entity ground_body = m_scene->CreateEntity(Ren::TransformComponent({ 0.0f, -10.0f }), Ren::TagList{ "ground" });
			auto& ground_body_r = ground_body.Add<Ren::RigidBodyComponent>();
			ground_body_r.p_shape = box_shape;
			box_shape->SetAsBox(50.0f, 10.0f);

			Ren::Entity another_body = m_scene->CreateEntity({{ -1.7f, 2.0f }}, { "another_body" });
			auto& another_body_r = another_body.Add<Ren::RigidBodyComponent>();
			box_shape = new b2PolygonShape();
			box_shape->SetAsBox(1.0f, 1.0f);
			another_body_r.p_shape = box_shape;

			// Dynamic body
			Ren::Entity dynamic_body = m_scene->CreateEntity({{ 0.0f, 10.0f }}, { "dynamic_body" });
			auto& dynamic_body_r = dynamic_body.Add<Ren::RigidBodyComponent>();
			box_shape = new b2PolygonShape();
			box_shape->SetAsBox(1.5f, 1.f);
			dynamic_body_r.p_shape = box_shape;
			dynamic_body_r.body_def.type = b2_dynamicBody;
			dynamic_body_r.fixture_def.density = 1.0f;
			dynamic_body_r.fixture_def.friction = 0.3f;
			dynamic_body_r.fixture_def.restitution = 0.67;
		}
		// Call init on all scene subsystems.
		m_scene->Init();
    }
    void OnDestroy() override
    {
        // Destroy native script component.
		m_scene->Destroy();

		delete m_scene;
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
			auto box_shape = new b2PolygonShape();
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