#include "Ren/GameCore.h"
#include "Ren/Core.h"
#include <algorithm>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

using namespace Ren;

///////////////////////
/////// INIT //////////
///////////////////////
void GameCore::Init(const GameDefinition& def)
{
    m_gameDefinition = def;

    // Initialize subsystems.
    m_context.Init(m_gameDefinition.context_def);
    if (def.init_flags & REN_INIT_IMGUI)
        m_imguiContext.Init(m_context.window, m_context.renderer, m_gameDefinition.imgui_def);
    if (def.init_flags & REN_INIT_BOX2D)
        init_box2d();
    
    // Save init ticks, so that first delta isn't as big as initialization time.
    m_lastFrameTicks = SDL_GetTicks();

    // User defined init.
    onInit();

    m_initialized = true;
    REN_STATUS("Game core initialized.");
}

void GameCore::init_box2d()
{
    REN_STATUS("Box2D initialization not implemented yet.");
}

///////////////////////
///// Destruction /////
///////////////////////
void GameCore::Destroy()
{
    // User defined destroy.
    onDestroy();

    // Destroy subsystems.
    if (m_gameDefinition.init_flags & REN_INIT_IMGUI)
        m_imguiContext.Destroy();
    if (m_gameDefinition.init_flags & REN_INIT_BOX2D)
        destroy_box2d();
    m_context.Destroy();
    
    m_initialized = false;
    REN_STATUS("Game core destroyed.");
}
void GameCore::destroy_box2d()
{
    REN_STATUS("Box2D destruction not implemented yet.");
}

////////////////////////
//////// Other /////////
////////////////////////
void GameCore::Loop()
{
    REN_ASSERT(m_initialized, "Game not initialized! Call GameCore::Init() first.");


    while (m_run)
    {
        // Calculate delta time.
        uint64_t current_ticks = SDL_GetTicks64();
        uint64_t ticks_delta = current_ticks - m_lastFrameTicks;
        m_lastFrameTicks = current_ticks;
        float delta_time = ticks_delta / 1000.0f;   // Convert from milliseconds to seconds.

        // Clear window.
        SDL_SetRenderDrawColor(m_context.renderer, m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        SDL_RenderClear(m_context.renderer);

        // Poll events.
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
            m_input.OnEvent(e);
            onEvent(e);
        }
        
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // User defined update.
        onUpdate(delta_time);

        // User defined render.
        onRender(m_context.renderer);
        
        // User defined imgui actions.
        onImGui(m_imguiContext);
        
        // Render Dear ImGui.
        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

        // Update the screen (swap back and front buffers).
        SDL_RenderPresent(m_context.renderer);
    }
}