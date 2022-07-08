#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "config.h"
#include "test_dir/dummy.h"

// Window dimension constants.
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

int main(int argc, char **argv) 
{
	Logger::Log("Hello, world!");

	SDL_Window* window = NULL;
	SDL_Surface* window_surface = NULL;
		
	// Init SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::printf("Failed to initialize SDL. SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	// Create window.
	uint32_t window_flags = SDL_WINDOW_SHOWN;
	window = SDL_CreateWindow("SDL first window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
	if (!window)
	{
		std::printf("Failed to create SDL window! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	// Initialize PNG loading.
	uint32_t img_init_flags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(img_init_flags) & img_init_flags))
	{
		std::printf("Failed to initilize SDL_image! Error: %s\n", IMG_GetError());
		return 1;
	}

	// Create renderer for hardware accelleartion.
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Create texture from loaded_img.
	SDL_Texture* texture = IMG_LoadTexture(renderer, ASSETS_DIR "awesomeface.png");
	
	// Game loop.
	bool run = true;
	while (run)
	{
		// Poll events.
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				run = false;
			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
					run = false;
			}
		}

		// Clean the screen.
		SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
		SDL_RenderClear(renderer);

		// Render texture.
		SDL_Rect rect{ 100, 100, 200, 200 };
		SDL_RenderCopy(renderer, texture, NULL, &rect);

		// Update the screen (swap back buffer with front buffer).
		SDL_RenderPresent(renderer);
	}

	// Free resources and quit SDL.
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
