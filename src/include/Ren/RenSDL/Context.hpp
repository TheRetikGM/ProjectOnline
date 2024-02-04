#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#define INVALID_WINDOW_SIZE glm::ivec2(-1, -1)
#define WINDOWPOS_UNDEFINED glm::ivec2(SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED)
#define WINDOWPOS_CENTERED  glm::ivec2(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)

namespace Ren {
    struct SDLContextDef {
        glm::ivec2 window_pos = WINDOWPOS_UNDEFINED;
        glm::ivec2 window_size = INVALID_WINDOW_SIZE;
        std::string window_name{ "Window name" };
        uint32_t sdl_init_flags{ SDL_INIT_VIDEO | SDL_INIT_TIMER };
        uint32_t img_init_flags{ IMG_INIT_PNG | IMG_INIT_JPG };
        uint32_t window_flags{ SDL_WINDOW_SHOWN };
        uint32_t renderer_flags{ SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC };
    };
    struct SDLContext {
        SDL_Window*     window{ nullptr };
        SDL_Renderer*   renderer{ nullptr };
        SDLContextDef definition;

        SDLContext() = default;
        ~SDLContext() {}

        // Create context with default presets (only window pos is required for default config).
        inline void Init(glm::ivec2 window_size) { Init(SDLContextDef{ WINDOWPOS_UNDEFINED, window_size }); }

        // Create context with custom presets.
        void Init(const SDLContextDef& def);

        // If you called Init(), then you should call this to destroy context.
        void Destroy();
    };

    ////////////////////////////
    //////// Dear ImGui ////////
    ////////////////////////////
    // Check https://github.com/ocornut/imgui/blob/master/examples/example_sdl_sdlrenderer/main.cpp for more info.

    // TODO: Implement custom styling.
    // FIXME: Name collides with ImGuiStyle from imgui.h. For now it is fixed by using namespace Ren, but it would be cleaner to think of another name.
    enum class ImGuiStyle : uint8_t { dark, light, classic };

    struct ImGuiContextDef {
        Ren::ImGuiStyle style{ ImGuiStyle::dark };
        ImGuiConfigFlags config_flags = ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    };
    // Imgui context.
    // FIXME: Name collides with ImGuiContext from imgui.h. For now it is fixed by using namespace Ren, but it would be cleaner to think of another name.
    struct ImGuiContext {
        ImGuiContextDef definition;
        ImGuiIO* io;

        // Create context with default presets.
        inline void Init(SDL_Window* window, SDL_Renderer* renderer) { Init(window, renderer, ImGuiContextDef()); };

        // Create context with custom presets.
        void Init(SDL_Window* window, SDL_Renderer* renderer, const ImGuiContextDef& def);

        // Destroy context.
        void Destroy();
    };
};
