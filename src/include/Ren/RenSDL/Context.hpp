#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <cstdint>
#include "Ren/Core.h"
#include <string>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#define INVALID_WINDOW_SIZE glm::ivec2(-1, -1)
#define WINDOWPOS_UNDEFINED glm::ivec2(SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED)
#define WINDOWPOS_CENTERED glm::ivec2(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)

namespace Ren
{
    struct SDLContextDef
    {
        glm::ivec2 window_pos = WINDOWPOS_UNDEFINED;
        glm::ivec2 window_size = INVALID_WINDOW_SIZE;
        std::string window_name{ "Window name" };
        uint32_t sdl_init_flags{ SDL_INIT_VIDEO | SDL_INIT_TIMER };
        uint32_t img_init_flags{ IMG_INIT_PNG | IMG_INIT_JPG };
        uint32_t window_flags{ SDL_WINDOW_SHOWN };
        uint32_t renderer_flags{ SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC };
    };
    struct SDLContext
    {
        SDL_Window*     window{ nullptr };
        SDL_Renderer*   renderer{ nullptr };
        SDLContextDef definition;

        SDLContext() = default;
        ~SDLContext() {}

        // Create context with default presets (only window pos is required for default config).
        inline void Init(glm::ivec2 window_size) { Init(SDLContextDef{ WINDOWPOS_UNDEFINED, window_size }); }

        // Create context with custom presets.
        inline void Init(const SDLContextDef& def)
        {
            // Initialize SDL
            REN_ASSERT(SDL_Init(def.sdl_init_flags) >= 0, "SDL initialization failed! Error: " + std::string(SDL_GetError()));

            // Create Window
            REN_ASSERT(def.window_size != INVALID_WINDOW_SIZE, "Window size is not set!");
            window = SDL_CreateWindow(def.window_name.c_str(), def.window_pos.x, def.window_pos.y, def.window_size.x, def.window_size.y, def.window_flags);
            REN_ASSERT(window, "Window creation failed! Error: " + std::string(SDL_GetError()));

            // Initialize IMG loading.
            REN_ASSERT(IMG_Init(def.img_init_flags) & def.img_init_flags, "SDL_image not initialized! Error: " + std::string(IMG_GetError()));

            // Create renderer.
            renderer = SDL_CreateRenderer(window, -1, def.renderer_flags);

            REN_STATUS("SDL context created.");
        }

        // If you called Init(), then you should call this to destroy context.
        inline void Destroy()
        {
            SDL_DestroyRenderer(renderer);
            IMG_Quit();
            SDL_DestroyWindow(window);
            SDL_Quit();

            REN_STATUS("SDL context destroyed.");
        }
    };

    ////////////////////////////
    //////// Dear ImGui ////////
    ////////////////////////////
    // Check https://github.com/ocornut/imgui/blob/master/examples/example_sdl_sdlrenderer/main.cpp for more info.
    //       ^ is also referenced in Loop() and destroy_imgui();

    // TODO: Implement custom styling.
    enum class ImGuiStyle : uint8_t { dark, light, classic };

    struct ImGuiContextDef
    {
        Ren::ImGuiStyle style{ ImGuiStyle::dark };
        ImGuiConfigFlags config_flags = ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    };
    // Imgui context.
    struct ImGuiContext
    {
        ImGuiContextDef definition;
        ImGuiIO* io;

        // Create context with default presets.
        inline void Init(SDL_Window* window, SDL_Renderer* renderer) { Init(window, renderer, ImGuiContextDef()); };

        // Create context with custom presets.
        void Init(SDL_Window* window, SDL_Renderer* renderer, const ImGuiContextDef& def)
        {
            this->definition = def;

            REN_ASSERT(window != nullptr, "Invalid window");
            REN_ASSERT(renderer != nullptr, "Invalid renderer");

            // Setup Dear ImGui context.
            ImGui::CreateContext();
            io = &ImGui::GetIO(); (void)(*io);
            io->ConfigFlags |= def.config_flags;
            
            // Setup Dear ImGui style
            switch (def.style)
            {
                case ImGuiStyle::dark: ImGui::StyleColorsDark(); break;
                case ImGuiStyle::light: ImGui::StyleColorsLight(); break;
                case ImGuiStyle::classic: ImGui::StyleColorsClassic(); break;
            }

            // Setup Platform/Renderer backends
            ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
            ImGui_ImplSDLRenderer_Init(renderer);

            REN_STATUS("Dear ImGui context created.");
        }

        // Destroy context.
        void Destroy()
        {
            ImGui_ImplSDLRenderer_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();

            REN_STATUS("Dear ImGui context destroyed.");
        }
    };
};