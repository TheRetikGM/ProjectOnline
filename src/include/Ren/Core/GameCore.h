#pragma once
extern "C" {
    #include <SDL.h>
}
#include <cstdint>
#include <glm/glm.hpp>
#include <imgui.h>

#include "Ren/RenSDL/Context.hpp"
#include "Ren/Core/LayerStack.h"
#include "Ren/Core/Input.hpp"

#define REN_INIT_IMGUI (1 << 0)
#define REN_INIT_BOX2D (1 << 1)
// TODO: Implement these
/// #define REN_INIT_FREETYPE (1 << 2)

namespace Ren {
    // Could be loaded from file or sth.
    struct GameDefinition {
        /// Define window and SDL specs.
        SDLContextDef   context_def{};
        /// Init flags for the Ren engine.
        uint32_t        init_flags{ REN_INIT_IMGUI | REN_INIT_BOX2D };
        /// Definition of the Ren imgui context.
        ImGuiContextDef imgui_def{};
    };

    namespace Utils {
        inline bool key_pressed(const SDL_Event& e, SDL_Keycode key) { return e.type == SDL_KEYDOWN && e.key.keysym.sym == key && e.key.repeat == 0; }
        inline bool key_released(const SDL_Event& e, SDL_Keycode key) { return e.type == SDL_KEYUP && e.key.keysym.sym == key && e.key.repeat == 0; }
    }

    /*
        Class which acts as a Core of the whole game.
        It does several things:
            - Initialize and destroys subsystems ( SDL, ImGui, ...)
            - Manages user defined layers. (OnUpdate(), OnEvent(), OnRender(), ...)
            - Manages main game loop.
                - Polls events
                - Updates all layers
                - Flushes SDL renderer
    */
    class GameCore {
    public:
        // Set this to false, if you want to end loop.
        bool         m_Run{ true };
        glm::ivec4   m_ClearColor{ 0x0, 0x0, 0x0, 0xff };
        KeyInterface m_Input;

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

        // Push layer onto the stack. Note: Overlays are always above layers.
        void PushLayer(Ref<Layer> layer);
        // Push overlay layer onto the stack. Note: Overlays are always above layers.
        void PushOverlay(Ref<Layer> layer);
        inline LayerStack& GetLayerStack() { return m_layerStack; };

        /* User-defined functions */

        virtual void OnInit() {}
        virtual void OnUpdate(float dt) {}
        // FIXME: Not implemented yet.
        virtual void OnFixedUpdate(float dt) {}
        virtual void OnRender(SDL_Renderer* renderer) {}
        virtual void OnImGui(Ren::ImGuiContext& context) {}
        virtual void OnEvent(const SDL_Event& e) {}
        virtual void OnDestroy() {}

        inline const SDLContext& GetContext() { return m_context; }
        inline glm::ivec2 GetWindowSize() { return m_context.definition.window_size; }
        inline SDL_Renderer* GetRenderer() { return m_context.renderer; }

    private:
        bool            m_initialized{ false };
        SDLContext      m_context;
        GameDefinition  m_gameDefinition;
        uint64_t        m_lastFrameTicks{ 0 };
        LayerStack      m_layerStack{};

        // Dear ImGui context.
        ImGuiContext m_imguiContext;

        void init_box2d();
        void destroy_box2d();
    };
};
