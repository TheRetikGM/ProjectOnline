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
protected:
	void onInit() override
	{
		// Create texture from loaded_img.
		m_texture = IMG_LoadTexture(getRenderer(), ASSETS_DIR "awesomeface.png");

		setClearColor({ 100, 100, 100, 255 });
	}
	void onDestroy() override
	{
		SDL_DestroyTexture(m_texture);
	}
	void onEvent(const SDL_Event& e)
	{
		if (e.type == SDL_QUIT)
				m_run = false;
		if (e.type == SDL_KEYDOWN)
		{
			if (e.key.keysym.sym == SDLK_ESCAPE)
				m_run = false;
		}
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
		//std::printf("Delta: %f, Rect pos: [%f, %f]\n", dt, m_rectPos.x, m_rectPos.y);
	}
	void onRender(SDL_Renderer* renderer) override
	{
		// Render texture.
		SDL_Rect rect{ (int)m_rectPos.x, (int)m_rectPos.y, 200, 200 };
		SDL_RenderCopy(renderer, m_texture, NULL, &rect);		
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
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception caught! Message: " << e.what() << '\n';
	}
	catch(...)
	{
		std::cerr << "Invalid exception caught!" << '\n';
	}
	
	game.Destroy();
	return 0;
}
