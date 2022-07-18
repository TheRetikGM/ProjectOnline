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
public:
	Game(const Ren::GameDefinition& def) : Ren::GameCore(def) 
	{
		// NOTE: Here could be: Rendering layer, network layer, audio layer etc.
		PushLayer(new DemoLayer("Demo layer"));
	}
};

Ren::GameCore* CreateGame()
{
	Ren::GameDefinition def;
	def.context_def.window_size = { WINDOW_WIDTH, WINDOW_HEIGHT };
	def.context_def.window_name = "Demo project";
	
	return new Game(def);
}
		