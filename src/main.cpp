#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include "config.h"
#include "Ren/GameCore.h"

// Window dimension constants.
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

// EnTT Resource Management.
struct TextureResource 
{ 
	SDL_Texture* texture{ nullptr }; 
	glm::ivec2 size;

	TextureResource(SDL_Texture* tex, glm::ivec2 size)
		: texture(tex), size(size) {}
};
struct TextureLoader
{
	using result_type = std::shared_ptr<TextureResource>;
	
	static void TextureDeleter(TextureResource* texture) 
	{
		SDL_DestroyTexture(texture->texture);
		delete texture;
	};

	// Loader for sprite to be loaded from disk.
	result_type operator()(SDL_Renderer* renderer, std::string path_to_texture)
	{
		// Load texture and get it's size.
		auto texture_surface{ IMG_Load(path_to_texture.c_str()) };
		auto texture{ SDL_CreateTextureFromSurface(renderer, texture_surface) };
		REN_ASSERT(texture != nullptr, "Failed to load texture. Error: " + std::string(IMG_GetError()));
		glm::ivec2 texture_size{ texture_surface->w, texture_surface->h };
		SDL_FreeSurface(texture_surface);

		// Shared_ptr with custom deleter, which properly deletes the texture.
		return std::shared_ptr<TextureResource>(new TextureResource(texture, texture_size), TextureDeleter);
	}

	// Loader for texture, which is generated from some text.
	result_type operator()(SDL_Renderer* renderer, TTF_Font* font, std::string text, glm::ivec4 color)
	{
		auto text_surface{ TTF_RenderText_Solid(font, text.c_str(), { (uint8_t)color.r, (uint8_t)color.g, (uint8_t)color.b, (uint8_t)color.a }) };
		REN_ASSERT(text_surface != nullptr, "Cannot generate surface from text! Error: " + std::string(TTF_GetError()));
		glm::ivec2 size{ text_surface->w, text_surface->h };
		auto texture{ SDL_CreateTextureFromSurface(renderer, text_surface) };
		SDL_FreeSurface(text_surface);

		return std::shared_ptr<TextureResource>(new TextureResource(texture, size), TextureDeleter);
	}
};
using TextureCache = entt::resource_cache<TextureResource, TextureLoader>;
using namespace entt::literals;

// Components.
struct TransformComponent
{
	glm::vec2 position{ .0f, .0f };
	glm::vec2 scale { .0f, .0f };
	int32_t layer{ 0 };
	
	TransformComponent(glm::vec2 pos = glm::vec2(0.0f), glm::vec2 scale = glm::vec2(10.0f), int32_t layer = 0)
		: position(pos), scale(scale), layer(layer) {}
};
struct TextureComponent
{
	entt::resource<TextureResource> texture_handler;
	bool outline = false;
};

class Game : public Ren::GameCore
{
	glm::vec2 m_rectPos{ 100, 100 };
	TTF_Font* m_font{ nullptr };
	SDL_Texture* m_textTexture{ nullptr };
	glm::ivec2 m_textTextureSize{ 0, 0 };

	// EnTT //
	// Stores all entities and manages their creation etc.
	entt::registry m_registry{ entt::registry() };
	// Example entity.
	entt::entity m_ent{ 0 };
	// Cache for storing loaded texture resources.
	TextureCache m_textureCache{};
protected:
	void onInit() override
	{
		// Set background color.
		m_clearColor = { 100, 100, 100, 255 };

		// EnTT //
		auto ret = m_textureCache.load("assets/awesomeface"_hs, getRenderer(), ASSETS_DIR "awesomeface.png");
		REN_ASSERT(ret.second, "Texture resource was not loaded.");

		// Create awesomeface entity.
		m_ent = m_registry.create();
		m_registry.emplace<TransformComponent>(m_ent, glm::vec2(0.0f), glm::vec2(200.0f));
		auto& face_tex = m_registry.emplace<TextureComponent>(m_ent);
		face_tex.texture_handler = ret.first->second;
		face_tex.outline = true;

		// Load font and create entity with text texture.
		m_font = TTF_OpenFont(ASSETS_DIR "fonts/DejaVuSansCondensed.ttf", 24);

		auto text_ent{ m_registry.create() };
		ret = m_textureCache.load("texts/intro1"_hs, getRenderer(), m_font, "WSAD for movement, 'i' to toggle imgui demo window, ESC to exit", glm::ivec4(0x0, 0xff, 0x0, 0xff));
		REN_ASSERT(ret.second, "Failed to create text.");
		auto& trans = m_registry.emplace<TransformComponent>(text_ent);
		auto& tex = m_registry.emplace<TextureComponent>(text_ent);
		tex.texture_handler = ret.first->second;
		trans.position = { 10.0f, 10.0f };
		trans.scale = tex.texture_handler->size;
		trans.layer = 2;
	}
	void onDestroy() override
	{
		SDL_DestroyTexture(m_textTexture);
		TTF_CloseFont(m_font);

		m_textureCache.clear();
		m_registry.clear();
	}
	void onEvent(const SDL_Event& e)
	{
		m_run = !(Ren::Utils::key_pressed(e, SDLK_ESCAPE) || e.type == SDL_QUIT);
	}

	void onUpdate(float dt) override
	{
		auto& transform = m_registry.get<TransformComponent>(m_ent);

		float move_speed = 500.0f;
		if (m_input.KeyHeld(SDLK_w))
			transform.position.y -= move_speed * dt;
		if (m_input.KeyHeld(SDLK_s))
			transform.position.y += move_speed * dt;
		if (m_input.KeyHeld(SDLK_a))
			transform.position.x -= move_speed * dt;
		if (m_input.KeyHeld(SDLK_d))
			transform.position.x += move_speed * dt;
	}
	void onRender(SDL_Renderer* renderer) override
	{
		// Sory by layer and render all texture components.
		auto view = m_registry.view<TransformComponent, TextureComponent>();
		m_registry.sort<TransformComponent>([](const auto& lhs, const auto& rhs){
			return lhs.layer < rhs.layer;
		});
		for (auto&& ent : view)
		{
			auto [trans, tex] = view.get(ent);
			SDL_Rect rect{ (int)trans.position.x, (int)trans.position.y, (int)trans.scale.x, (int)trans.scale.y };
			SDL_RenderCopy(renderer, tex.texture_handler->texture, {}, &rect);

			if (tex.outline)
			{
				// Draw magenta outline.
				SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
				SDL_RenderDrawRect(renderer, &rect);
			}
		}
	}
	void onImGui(Ren::ImGuiContext& context) override
	{
		static bool show_demo = false;
		if (m_input.KeyPressed(SDLK_i))
			show_demo = !show_demo;
		
		if (show_demo)
			ImGui::ShowDemoWindow();
	}
};

int main(int argc, char **argv) 
{
	Game game;

	try
	{
		game.Init({ WINDOW_WIDTH, WINDOW_HEIGHT });
		game.Loop();
		game.Destroy();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception caught! Message: " << e.what() << '\n';
	}
	catch(...)
	{
		std::cerr << "Invalid exception caught!" << '\n';
	}
	
	return 0;
}
