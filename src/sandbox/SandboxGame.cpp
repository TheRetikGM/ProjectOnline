#include <iostream>
#include <Ren/Ren.h>

#include "scripts/Movement.hpp"

using namespace entt::literals;

// Window dimension constants.
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

struct OutlineComponent
{
	glm::ivec4 color = glm::ivec4(255);
};
class OutlineSystem : public Ren::ComponentSystem
{
public:
	OutlineSystem(Ren::Scene* p_scene, Ren::KeyInterface* p_input, SDL_Renderer* renderer) : Ren::ComponentSystem(p_scene, p_input), m_renderer(renderer) {}

	void Render() override
	{
		auto view = m_scene->SceneView<OutlineComponent, Ren::TransformComponent>();
		for (auto&& ent : view)
		{
			auto [outline, trans] = view.get(ent);
			SDL_Rect rect{ (int)trans.position.x, (int)trans.position.y, (int)trans.scale.x, (int)trans.scale.y };
			// Draw magenta outline.
			SDL_SetRenderDrawColor(m_renderer, outline.color.r, outline.color.g, outline.color.b, outline.color.a);
			SDL_RenderDrawRect(m_renderer, &rect);
		}
	}
private:
	SDL_Renderer* m_renderer = nullptr;
};


class Game : public Ren::GameCore
{
	glm::vec2 m_rectPos{ 100, 100 };
	TTF_Font* m_font{ nullptr };
	SDL_Texture* m_textTexture{ nullptr };
	glm::ivec2 m_textTextureSize{ 0, 0 };

	// EnTT //
	// Stores all entities and manages their creation etc.
	Ren::Scene* m_scene;
	// Example entity.
	Ren::Entity m_ent;
protected:
	void onInit() override
	{
		// Set background color.
		m_clearColor = { 100, 100, 100, 255 };

		m_scene = new Ren::Scene(getRenderer(), &m_input);
		m_scene->AddSystem<OutlineSystem>(getRenderer());

		// Create awesomeface entity.
		m_ent = m_scene->CreateEntity({ glm::vec2(0.0f), glm::vec2(200.0f) });
		m_ent.Add<Ren::SpriteComponent>(ASSETS_DIR "awesomeface.png").color = Ren::Colors3::Magenta;
		m_ent.Add<OutlineComponent>().color = Ren::Colors4::Cyan;
		m_ent.Add<Ren::NativeScriptComponent>().Bind<MovementScript>();

		// Create copy of awesomeface entity (the texture will get reused).
		auto ent_copy = m_scene->CreateEntity({ glm::vec2(0.0f), glm::vec2(200.0f) });
		ent_copy.Add<Ren::SpriteComponent>(ASSETS_DIR "awesomeface.png");
		ent_copy.Get<Ren::TagComponent>().tag = "rotate";

		// Load font and create entity with text texture.
		m_font = TTF_OpenFont(ASSETS_DIR "fonts/DejaVuSansCondensed.ttf", 24);
		REN_ASSERT(m_font != nullptr, "Failed opening font. Error: " + std::string(TTF_GetError()));
		auto ret = m_scene->GetTextureCache()->load("texts/intro1"_hs, getRenderer(), m_font, "WSAD for movement, 'i' to \n toggle imgui demo window, ESC to exit", glm::ivec4(0x0, 0xff, 0x0, 0xff));
		Ren::Entity text_ent = m_scene->CreateEntity();
		auto& trans = text_ent.Get<Ren::TransformComponent>();
		auto& tex = text_ent.Add<Ren::SpriteComponent>();
		tex.texture_handle = *ret.first;
		trans.position = { 10.0f, 10.0f };
		trans.scale = tex.GetTextureResource()->size;
		trans.layer = 2;

		// Call init on all scene subsystems.
		m_scene->Init();
	}
	void onDestroy() override
	{
		// Destroy native script component.
		m_scene->Destroy();

		SDL_DestroyTexture(m_textTexture);
		TTF_CloseFont(m_font);

		delete m_scene;
	}
	void onEvent(const SDL_Event& e)
	{
		m_run = !(Ren::Utils::key_pressed(e, SDLK_ESCAPE) || e.type == SDL_QUIT);
	}

	void onUpdate(float dt) override
	{
		// Rotate entities with 'rotate' tag.
		const float rotation_speed = 90.0f;		// 90 degrees per second.
		auto entities = m_scene->GetEntitiesByTag("rotate");
		for (auto&& ent : *entities)
			ent.Get<Ren::TransformComponent>().rotation += rotation_speed * dt;

		m_scene->Update(dt);
	}
	void onRender(SDL_Renderer* renderer) override
	{
		m_scene->Render();
	}
	void onImGui(Ren::ImGuiContext& context) override
	{
		static bool show_demo = false;
		if (m_input.KeyPressed(SDLK_i))
			show_demo = !show_demo;
		
		if (show_demo)
			ImGui::ShowDemoWindow();
	}
public:
	Game(const Ren::GameDefinition& def) : Ren::GameCore(def) {}
	Game(glm::ivec2 window_size) : Ren::GameCore(window_size) {}
};


Ren::GameCore* CreateGame()
{
	Ren::GameDefinition def;
	def.context_def.window_size = { WINDOW_WIDTH, WINDOW_HEIGHT };
	def.context_def.window_name = "Demo project";
	
	return new Game(def);
}
