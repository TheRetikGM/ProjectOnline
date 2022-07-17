#pragma once
#include <SDL2/SDL.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <imgui.h>
#include "RenSDL/Context.hpp"
#include "KeyInterface.hpp"

#define REN_INIT_IMGUI (1 << 0)
#define REN_INIT_BOX2D (1 << 1)
// TODO: Implement these
// #define REN_INIT_FREETYPE (1 << 2)
// #define REN_INIT_ENTT     (1 << 3)

namespace Ren
{
    // Could be loaded from file or sth.
    struct GameDefinition
    {
        // Define window and SDL specs.
        SDLContextDef   context_def;
        // Init flags for the Ren engine.
        uint32_t        init_flags{ REN_INIT_IMGUI | REN_INIT_BOX2D };
        // Definition of the Ren imgui context.
        ImGuiContextDef imgui_def;
    };

    namespace Utils
    {
        inline bool key_pressed(const SDL_Event& e, SDL_Keycode key) { return e.type == SDL_KEYDOWN && e.key.keysym.sym == key && e.key.repeat == 0; }
        inline bool key_released(const SDL_Event& e, SDL_Keycode key) { return e.type == SDL_KEYUP && e.key.keysym.sym == key && e.key.repeat == 0; }
    }

    class GameCore
    {
    public:
        // Create game core with default settings.
        GameCore(glm::ivec2 window_size) : GameCore(GameDefinition{ {WINDOWPOS_UNDEFINED, window_size} }) {}
        // Create game core with custom settings.
        GameCore(const GameDefinition& def);
        virtual ~GameCore();

        // Init game core.
        void Init();
        void Destroy();
        // Begin main loop. To exit set m_run to false.
        void Loop();

    protected:
        // Set this to false, if you want to end loop.
        bool         m_run{ true };
        glm::ivec4   m_clearColor{ 0x0, 0x0, 0x0, 0xff };
        KeyInterface m_input;

        /* User-defined functions */

        virtual void onInit() {}
        virtual void onUpdate(float dt) {}
        // FIXME: Not implemented yet.
        virtual void onFixedUpdate(float dt) {}
        virtual void onRender(SDL_Renderer* renderer) {}
        virtual void onImGui(Ren::ImGuiContext& context) {}
        virtual void onEvent(const SDL_Event& e) {}
        virtual void onDestroy() {}

        inline const SDLContext& getContext() { return m_context; }
        inline glm::ivec2 getWindowSize() { return m_context.definition.window_size; }
        inline SDL_Renderer* getRenderer() { return m_context.renderer; }

    private:
        bool            m_initialized{ false };
        SDLContext      m_context;
        GameDefinition  m_gameDefinition;
        uint64_t        m_lastFrameTicks{ 0 };

        // Dear ImGui context.
        ImGuiContext m_imguiContext;

        void init_box2d();
        void destroy_box2d();
    };
};