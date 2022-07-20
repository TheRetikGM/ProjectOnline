#pragma once
#include <Ren/Ren.h>
#include <yaml-cpp/yaml.h>

#include "scripts/Movement.hpp"

using namespace entt::literals;

struct OutlineComponent
{
	glm::ivec4 color = glm::ivec4(255);
};
struct OutlineRenderCommand
{
	Ren::TransformComponent& trans;
	OutlineComponent& outline;

	OutlineRenderCommand(Ren::Entity ent)
		: trans(ent.Get<Ren::TransformComponent>())
		, outline(ent.Get<OutlineComponent>())
	{}
	
	inline int GetLayer() const { return trans.layer; }
	void Render(SDL_Renderer* renderer)
	{
		SDL_Rect rect{ (int)trans.position.x, (int)trans.position.y, (int)trans.scale.x, (int)trans.scale.y };
		SDL_SetRenderDrawColor(renderer, outline.color.r, outline.color.g, outline.color.b, outline.color.a);
		SDL_RenderDrawRect(renderer, &rect);
	}
};
class OutlineSystem : public Ren::ComponentSystem
{
public:
	OutlineSystem(Ren::Scene* p_scene, Ren::KeyInterface* p_input, SDL_Renderer* renderer) : Ren::ComponentSystem(p_scene, p_input), m_renderer(renderer) {}

	void Render() override
	{
		auto view = m_scene->SceneView<OutlineComponent, Ren::TransformComponent>();
		for (auto&& ent : view)
			Ren::Renderer::SubmitCommand(OutlineRenderCommand{Ren::Entity{ ent, m_scene }});
	}
private:
	SDL_Renderer* m_renderer = nullptr;
};

class DemoLayer : public Ren::Layer
{
    glm::vec2 m_rectPos{ 100, 100 };
	Ref<Ren::TextRenderer> m_textRenderer = Ren::TextRenderer::Create();

	// EnTT //
	// Stores all entities and manages their creation etc.
	Ren::Scene* m_scene;
	// Example entity.
	Ren::Entity m_ent;
public:
    DemoLayer(const std::string& name) : Ren::Layer(name) {}

    void OnInit() override
    {
        // Set background color.
		m_GameCore->m_ClearColor = { 100, 100, 100, 255 };

		m_scene = new Ren::Scene(GetRenderer(), GetInput());
		m_scene->AddSystem<OutlineSystem>(GetRenderer());

		// Create awesomeface entity.
		m_ent = m_scene->CreateEntity({ glm::vec2(0.0f), glm::vec2(200.0f) });
		m_ent.Add<Ren::SpriteComponent>(ASSETS_DIR "awesomeface.png").color = Ren::Colors3::Magenta;
		m_ent.Add<OutlineComponent>().color = Ren::Colors4::Cyan;
		m_ent.Add<Ren::NativeScriptComponent>().Bind<MovementScript>();
		m_ent.Get<Ren::TagComponent>().tag = "awesomeface";
		m_ent.Get<Ren::TransformComponent>().layer = 2;

		// Create copy of awesomeface entity (the texture will get reused).
		auto ent_copy = m_scene->CreateEntity({ glm::vec2(0.0f), glm::vec2(200.0f) });
		ent_copy.Add<Ren::SpriteComponent>(ASSETS_DIR "awesomeface.png");
		ent_copy.Get<Ren::TagComponent>().tag = "rotate";
		ent_copy.Get<Ren::TransformComponent>().layer = 1;

		// Call init on all scene subsystems.
		m_scene->Init();

		m_textRenderer->Load(ASSETS_DIR "fonts/DejaVuSansCondensed.ttf", 32);
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
    }
    void OnUpdate(float dt) override
    {
        // Rotate entities with 'rotate' tag.
		const float rotation_speed = 90.0f;		// 90 degrees per second.
		auto entities = m_scene->GetEntitiesByTag("rotate");
		for (auto&& ent : *entities)
			ent.Get<Ren::TransformComponent>().rotation += rotation_speed * dt;

		m_scene->Update(dt);
    }
    void OnRender(SDL_Renderer* renderer) override
    {
		Ren::Renderer::BeginRender();
		m_textRenderer->RenderText("WSAD for movement\n'i' to toggle imgui demo window\nESC to exit", { 10.0f, 10.0f }, 1.0f, Ren::Colors3::White, 10);
        m_scene->Render();
		Ren::Renderer::Render();
    }
    void OnImGui(Ren::ImGuiContext& context) override
    {
        static bool show_demo = false;
		if (KeyPressed(SDLK_i))
			show_demo = !show_demo;
		
		if (show_demo)
			ImGui::ShowDemoWindow();
    }
};