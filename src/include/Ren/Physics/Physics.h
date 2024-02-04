#pragma once
#include <cstdint>
#include <glm/glm.hpp>

#include "box2d/box2d.h"

namespace Ren {
    namespace Utils {
        inline glm::vec2 to_vec2(const b2Vec2& vec) { return *(glm::vec2*)(&vec); }
        inline b2Vec2 to_b2Vec2(const glm::vec2& vec) { return *(b2Vec2*)(&vec); }
    }
}
