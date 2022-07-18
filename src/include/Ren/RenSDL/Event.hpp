#pragma once
#include <SDL2/SDL.h>

namespace Ren
{
    struct Event
    {
        const SDL_Event& sdl_event;
        bool handled{ false };
    };
} // namespace Ren
