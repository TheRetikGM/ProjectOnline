/**
 * @file Ren/Core/GameCore.cpp
 * @brief Implementation of core functionality for create engine.
 *
 * This includes creation of context and main loop.
 */
#include "Ren/Core/GameCore.hpp"
#include "Ren/Core/Core.hpp"
#include <algorithm>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "Ren/Core/Layer.hpp"
#include "Ren/Renderer/Renderer.hpp"

using namespace Ren;

GameCore::GameCore(const GameDefinition& def)
    : m_gameDefinition(def)
{
    // Initialize subsystems.
    m_context.Init(m_gameDefinition.context_def);
    if (def.init_flags & REN_INIT_IMGUI)
        m_imguiContext.Init(m_context.window, m_context.renderer, m_gameDefinition.imgui_def);
    if (def.init_flags & REN_INIT_BOX2D)
        init_box2d();

    Renderer::SetRenderer(m_context.renderer);
}
GameCore::~GameCore() {
    // Destroy subsystems.
    if (m_gameDefinition.init_flags & REN_INIT_IMGUI)
        m_imguiContext.Destroy();
    if (m_gameDefinition.init_flags & REN_INIT_BOX2D)
        destroy_box2d();
    m_context.Destroy();
    Renderer::SetRenderer(nullptr);
}

///////////////////////
/////// INIT //////////
///////////////////////
void GameCore::Init() {
    // Save init ticks, so that first delta isn't as big as initialization time.
    m_lastFrameTicks = SDL_GetTicks();

    // Call user defined inits on all layers and core layer.
    OnInit();
    for (auto&& layer : m_layerStack)
        layer->OnInit();

    m_initialized = true;
    REN_STATUS("Game core initialized.");
}

void GameCore::init_box2d() {
    REN_STATUS("Box2D initialization not implemented yet.");
}

///////////////////////
///// Destruction /////
///////////////////////
void GameCore::Destroy() {
    // User destroy all layers in reverse order.
    for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); it++)
        (*it)->OnDestroy();
    OnDestroy();

    m_layerStack.clear();
    m_initialized = false;
    REN_STATUS("Game core destroyed.");
}

void GameCore::destroy_box2d() {
    REN_STATUS("Box2D destruction not implemented yet.");
}

////////////////////////
//////// Other /////////
////////////////////////
void GameCore::Loop() {
    REN_ASSERT(m_initialized, "Game not initialized! Call GameCore::Init() first.");


    while (m_Run) {
        // Calculate delta time.
        uint64_t current_ticks = SDL_GetTicks64();
        uint64_t ticks_delta = current_ticks - m_lastFrameTicks;
        m_lastFrameTicks = current_ticks;
        float delta_time = ticks_delta / 1000.0f;   // Convert from milliseconds to seconds.

        // Clear window.
        SDL_SetRenderDrawColor(m_context.renderer, m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
        SDL_RenderClear(m_context.renderer);

        // Poll events.
        SDL_Event e;
        // Event processing order: ImGui -> Layers in reverse (overlay -> normal layers) -> core layer -> input
        while (SDL_PollEvent(&e)) {
            // If the event is QUIT, then don't process it any further. We always want a way to the close application without killing it.
            if (e.type == SDL_QUIT) {
                m_Run = false;
                break;
            }
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
                m_context.definition.window_size = m_gameDefinition.context_def.window_size = { e.window.data1, e.window.data2 };

            // TODO: Check if imgui used the event and prevent other layers from accessing it.
            ImGui_ImplSDL2_ProcessEvent(&e);

            Event ren_event{ e, false };
            // Pass events to all layers in reverse order, so that overlay layers will get the event first.
            for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); it++)
            {
                std::string name = (*it)->m_name;
                (*it)->OnEvent(ren_event);
                if (ren_event.handled)
                    break;
            }
            if (!ren_event.handled)
                OnEvent(e);
            if (!ren_event.handled)
                m_Input.OnEvent(e);
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // User defined update of all layers.
        for (auto&& layer : m_layerStack)
            layer->OnUpdate(delta_time);
        OnUpdate(delta_time);

        // User defined render of all layers.
        for (auto&& layer : m_layerStack)
            layer->OnRender(m_context.renderer);
        OnRender(m_context.renderer);

        // User defined imgui actions of all layers.
        for (auto&& layer : m_layerStack)
            layer->OnImGui(m_imguiContext);
        OnImGui(m_imguiContext);

        // Render Dear ImGui.
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

        // Update the screen (swap back and front buffers).
        SDL_RenderPresent(m_context.renderer);
    }
}

void GameCore::PushLayer(Ref<Layer> layer) { m_layerStack.PushLayer(layer); layer->m_GameCore = this; }
void GameCore::PushOverlay(Ref<Layer> layer) { m_layerStack.PushOverlay(layer); layer->m_GameCore = this; }
