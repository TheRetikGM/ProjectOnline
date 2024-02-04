/**
 * @brief Implementation of Ren context.
 * @file Ren/RenSDL/Context.cpp
 */
#include "Ren/RenSDL/Context.hpp"
#include "Ren/Core/Core.h"

using namespace Ren;

void SDLContext::Init(const SDLContextDef& def) {
    this->definition = def;

    // Initialize SDL
    int init_result = SDL_Init(def.sdl_init_flags);
    REN_ASSERT(init_result >= 0, "SDL initialization failed! Error: " + std::string(SDL_GetError()));

    // Create Window
    REN_ASSERT(def.window_size != INVALID_WINDOW_SIZE, "Window size is not set!");
    window = SDL_CreateWindow(def.window_name.c_str(), def.window_pos.x, def.window_pos.y, def.window_size.x, def.window_size.y, def.window_flags);
    REN_ASSERT(window != nullptr, "Window creation failed! Error: " + std::string(SDL_GetError()));

    // Initialize IMG loading.
    int img_init_result = IMG_Init(def.img_init_flags);
    REN_ASSERT(img_init_result & def.img_init_flags, "SDL_image not initialized! Error: " + std::string(IMG_GetError()));

    // Initialize TTF.
    int ttf_init_result = TTF_Init();
    REN_ASSERT(ttf_init_result != -1, "SDL_ttf initialization failed! TTF_Error: " + std::string(TTF_GetError()));

    // TODO: Initialize SDL_net and SDL_mixer.

    // Create renderer.
    renderer = SDL_CreateRenderer(window, -1, def.renderer_flags);

    REN_STATUS("SDL context created.");
}

void SDLContext::Destroy() {
    SDL_DestroyRenderer(renderer);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();

    REN_STATUS("SDL context destroyed.");
}

void Ren::ImGuiContext::Init(SDL_Window* window, SDL_Renderer* renderer, const ImGuiContextDef& def) {
    this->definition = def;

    REN_ASSERT(window != nullptr, "Invalid window");
    REN_ASSERT(renderer != nullptr, "Invalid renderer");

    // Setup Dear ImGui context.
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)(*io);
    io->ConfigFlags |= def.config_flags;

    // Setup Dear ImGui style
    switch (def.style) {
        case ImGuiStyle::dark: ImGui::StyleColorsDark(); break;
        case ImGuiStyle::light: ImGui::StyleColorsLight(); break;
        case ImGuiStyle::classic: ImGui::StyleColorsClassic(); break;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    REN_STATUS("Dear ImGui context created.");
}

void Ren::ImGuiContext::Destroy() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    REN_STATUS("Dear ImGui context destroyed.");
}

