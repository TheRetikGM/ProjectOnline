#pragma once
#include "Ren/Core/GameCore.h"

extern Ren::GameCore* CreateGame();

int main(int argc, char* argv[])
{
    Ren::GameCore* game = CreateGame();

    try
	{
		game->Init();
		game->Loop();
		game->Destroy();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception caught! Message: " << e.what() << '\n';
		return 1;
	}
	catch(...)
	{
		std::cerr << "Invalid exception caught!" << '\n';
		return 2;
	}

	delete game;
	return 0;
}