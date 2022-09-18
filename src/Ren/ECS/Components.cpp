#include <algorithm>

#include "Ren/ECS/Components.h"
#include "Ren/ECS/NativeScript.h"
#include "Ren/Utils/Logger.hpp"

namespace Ren
{
    glm::vec2 SpriteComponent::GetSize()
    {
        // Scale size of the texture in pixels to match size in units (as defined by SpriteComponent::m_PixelPerIUnit property).
        int w, h;
        SDL_QueryTexture(GetTexture(), nullptr, nullptr, &w, &h);
        glm::vec2 size = glm::vec2(w, h) / glm::vec2(m_PixelsPerUnit);
        return size;
    }

    void NativeScriptComponent::Unbind()
    {
        if (script_instance)
            delete script_instance;
    }
};