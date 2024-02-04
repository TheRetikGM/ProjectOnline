#pragma once
#include <SDL.h>
#include <glm/glm.hpp>

namespace Ren {
    struct Event {
        const SDL_Event& event;
        bool handled{ false };
        glm::vec2 mouse_offset{ 0.0f, 0.0f };
    };
} // namespace Ren
