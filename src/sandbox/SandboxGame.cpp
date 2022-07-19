#include <iostream>
#include <Ren/Ren.h>
#include <Ren/Layer.h>
#include <Ren/Renderer/Renderer.h>

#include "DemoLayer.hpp"

// Window dimension constants.
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

class Game : public Ren::GameCore
{
	Ren::Utils::FpsCounter m_fpsCounter;
public:
	Game(const Ren::GameDefinition& def) : Ren::GameCore(def) 
	{
		// NOTE: Here could be: Rendering layer, network layer, audio layer etc.
		PushLayer(new DemoLayer("Demo layer"));
	}

	void OnUpdate(float dt) override
	{
		m_fpsCounter.Update(dt);
	}

	void OnImGui(Ren::ImGuiContext& context) override
	{
		m_fpsCounter.DrawPlot(context, GetWindowSize());
	}
};

Ren::GameCore* CreateGame()
{
	Ren::GameDefinition def;
	def.context_def.window_size = { WINDOW_WIDTH, WINDOW_HEIGHT };
	def.context_def.window_name = "Demo project";
//	def.context_def.window_flags |= SDL_WINDOW_RESIZABLE;
	
	return new Game(def);
}
		