#pragma once
#include <Ren/Ren.h>
#include <Ren/Core/Camera.h>
#include <box2d/box2d.h>

#include "scripts/Movement.hpp"

using namespace entt::literals;

struct OutlineComponent
{
	glm::ivec4 color = glm::ivec4(255);
	float rotation = 0.0f;
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
			Ren::Renderer::DrawRect({ trans.position, trans.scale }, outline.rotation, outline.color);
		}
	}
private:
	SDL_Renderer* m_renderer = nullptr;
};

class DemoLayer : public Ren::Layer
{
	Ref<Ren::TextRenderer> m_textRenderer = Ren::TextRenderer::Create();
	Ren::Scene* m_scene;
	Ren::Entity m_ent;
	Ren::CartesianCamera m_camera;
	
	b2Vec2 m_gravity{ 0.0f, -9.81f };
	Ref<b2World> m_world{ nullptr };
	b2Body* m_body{ nullptr };
	b2Body* m_groundBody{ nullptr };
	b2Body* m_anotherBody{ nullptr };
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

		/////////// Box2D ///////////
		{
			m_world = std::make_shared<b2World>(m_gravity);

			// Ground
			b2BodyDef ground_body_def;
			ground_body_def.position.Set(0.0f, -10.0f);
			m_groundBody = m_world->CreateBody(&ground_body_def);
			b2PolygonShape ground_box;
			ground_box.SetAsBox(50.0f, 10.0f);
			m_groundBody->CreateFixture(&ground_box, 0.0f);  // Static body

			ground_box.SetAsBox(1.0f, 1.0f);
			ground_body_def.position.Set(-1.7f, 2.0f);
			m_anotherBody = m_world->CreateBody(&ground_body_def);
			m_anotherBody->CreateFixture(&ground_box, 0.0f);

			// Dynamic body
			b2BodyDef body_def;
			body_def.type = b2_dynamicBody;
			body_def.position.Set(0.0f, 10.0f);
			m_body = m_world->CreateBody(&body_def);

			b2PolygonShape dynamic_box;
			dynamic_box.SetAsBox(1.5f, 1.0f);

			b2FixtureDef body_fix_def;
			body_fix_def.shape = &dynamic_box;
			body_fix_def.density = 1.0f;
			body_fix_def.friction = 0.3;
			body_fix_def.restitution = 0.67f;

			m_body->CreateFixture(&body_fix_def);
		}
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
				m_camera.SetUnitScale(m_camera.GetUnitScale() + glm::ivec2(10));
			else
				m_camera.SetUnitScale(m_camera.GetUnitScale() - glm::ivec2(10));
		}
		if (e.sdl_event.type == SDL_MOUSEBUTTONDOWN && e.sdl_event.button.button == SDL_BUTTON_LEFT)
		{
			glm::ivec2 mouse_pos(0);
			SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
			glm::vec2 pos = m_camera.ToUnits(glm::vec2(mouse_pos));

			b2BodyDef body_def;
			body_def.type = b2_dynamicBody;
			body_def.position.Set(pos.x, pos.y);
			b2Body* body = m_world->CreateBody(&body_def);

			b2PolygonShape dynamic_box;
			dynamic_box.SetAsBox(Ren::Utils::RandomFloat(0.5f, 2.0f), Ren::Utils::RandomFloat(0.5f, 2.0f));

			b2FixtureDef body_fix_def;
			body_fix_def.shape = &dynamic_box;
			body_fix_def.density = Ren::Utils::RandomFloat(0.5f, 5.0f);
			body_fix_def.friction = Ren::Utils::RandomFloat_0_1();
			body_fix_def.restitution = Ren::Utils::RandomFloat_0_1();

			body->CreateFixture(&body_fix_def);
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

		auto outline_ents = m_scene->SceneView<OutlineComponent>();
		for (auto&& ent : outline_ents)
		{
			auto [outline] = outline_ents.get(ent);
			outline.rotation -= rotation_speed * dt * 1.3f;
			if (std::abs(outline.rotation) > 360.0f)
				outline.rotation += outline.rotation > 0.0f ? -360.0f : 360.0f;
		}

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

		m_world->Step(1.0f / 60.0f, 6, 2);
    }
    void OnRender(SDL_Renderer* renderer) override
    {
		Ren::Renderer::BeginRender(&m_camera);
		m_textRenderer->RenderText(
			"WSAD for movement\n"
			"Arrow keys or hold mouse right button for camera movement\n"
			"'i' to toggle imgui demo window\n"
			"ESC to exit\n"
			"Mouse left button for spawning new body"
			, { 10.0f, 10.0f }, 1.0f, Ren::Colors3::White, 10);
        m_scene->Render();

		const auto to_vec2 = [](b2Vec2 v){ return *(glm::vec2*)(&v); };
		const auto draw_body = [&](b2Body* body){
			b2PolygonShape* shape = (b2PolygonShape*)body->GetFixtureList()->GetShape();
			SAND_ASSERT(shape->m_count == 4, "Only rectangles are supported for now.");

			glm::vec2 pos = to_vec2(body->GetPosition());
			Ren::Rect rect{ pos + to_vec2(shape->m_vertices[0]), to_vec2(shape->m_vertices[2]) - to_vec2(shape->m_vertices[0]) };
			Ren::Renderer::DrawRect(rect, glm::degrees(body->GetAngle()), Ren::Colors4::White);
		};
		for (b2Body* body = m_world->GetBodyList(); body; body = body->GetNext())
			draw_body(body);

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