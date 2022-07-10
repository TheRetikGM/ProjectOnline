#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include "config.h"
#include "Ren/GameCore.h"

// Window dimension constants.
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

class Game : public Ren::GameCore
{
	SDL_Texture* m_texture{ nullptr };
	glm::vec2 m_rectPos{ 100, 100 };
	TTF_Font* m_font{ nullptr };
	SDL_Texture* m_textTexture{ nullptr };
	glm::ivec2 m_textTextureSize{ 0, 0 };
protected:
	void onInit() override
	{
		// Set background color.
		m_clearColor = { 100, 100, 100, 255 };

		// Create texture from loaded_img.
		m_texture = IMG_LoadTexture(getRenderer(), ASSETS_DIR "awesomeface.png");

		// Load font and create texture with text.
		m_font = TTF_OpenFont(ASSETS_DIR "fonts/DejaVuSansCondensed.ttf", 24);
		SDL_Surface* tmp_surface = TTF_RenderText_Solid(m_font, "WSAD for movement, 'i' to toggle imgui demo window, ESC to exit", { 0x0, 0xff, 0x0, 0xff });	// Green text.
		m_textTexture = SDL_CreateTextureFromSurface(getRenderer(), tmp_surface);
		m_textTextureSize = { tmp_surface->w, tmp_surface->h };
		SDL_FreeSurface(tmp_surface);
	}
	void onDestroy() override
	{
		SDL_DestroyTexture(m_textTexture);
		TTF_CloseFont(m_font);
		SDL_DestroyTexture(m_texture);
	}
	void onEvent(const SDL_Event& e)
	{
		m_run = !(Ren::Utils::key_pressed(e, SDLK_ESCAPE) || e.type == SDL_QUIT);
	}

	void onUpdate(float dt) override
	{
		float move_speed = 500.0f;
		if (m_input.KeyHeld(SDLK_w))
			m_rectPos.y -= move_speed * dt;
		if (m_input.KeyHeld(SDLK_s))
			m_rectPos.y += move_speed * dt;
		if (m_input.KeyHeld(SDLK_a))
			m_rectPos.x -= move_speed * dt;
		if (m_input.KeyHeld(SDLK_d))
			m_rectPos.x += move_speed * dt;
	}
	void onRender(SDL_Renderer* renderer) override
	{
		// Render texture.
		SDL_Rect rect{ (int)m_rectPos.x, (int)m_rectPos.y, 200, 200 };
		SDL_RenderCopy(renderer, m_texture, nullptr, &rect);
		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_RenderDrawRect(renderer, &rect);
		
		// Render text.
		rect = { 10, 10, m_textTextureSize.x, m_textTextureSize.y };
		SDL_RenderCopy(renderer, m_textTexture, nullptr, &rect);
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
