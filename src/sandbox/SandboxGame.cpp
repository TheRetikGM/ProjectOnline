#include <iostream>
#include <Ren/Ren.h>
#include <Ren/Core/Layer.h>
#include <Ren/Renderer/Renderer.h>

#include "DemoLayer.hpp"
#include "ImGuiLayer.hpp"

// Window dimension constants.
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

class Game : public Ren::GameCore
{
public:
    Game(const Ren::GameDefinition& def) : Ren::GameCore(def) 
    {
        auto demo_layer = CreateRef<DemoLayer>("Demo layer");

        // auto gui_layer =  CreateRef<ImGuiLayer>("gui layer");
        // gui_layer->m_DemoLayer = demo_layer;

        // NOTE: Here could be: Rendering layer, network layer, audio layer etc.
        PushLayer(demo_layer);
        // PushLayer(gui_layer);

        Ren::LogEmmiter::AddListener<Ren::StreamLogger>({ stdout });
    }
};

Ren::GameCore* CreateGame()
{
    Ren::GameDefinition def;
    def.context_def.window_size = { WINDOW_WIDTH, WINDOW_HEIGHT };
    def.context_def.window_name = "Demo project";
    def.context_def.window_flags |= SDL_WINDOW_RESIZABLE;

    return new Game(def);
}

