/**
 * @file Ren/ECS/Components.cpp
 * @brief Implementation of ECS components.
 */
#include <algorithm>
#include <ren_utils/logging.hpp>

#include "Ren/Core/Core.h"
#include "Ren/ECS/Components.h"
#include "Ren/Scripting/NativeScript.h"
#include "Ren/Scripting/LuaScript.h"

namespace Ren
{
    glm::vec2 SpriteComponent::GetSize() {
        // Scale size of the texture in pixels to match size in units (as defined by SpriteComponent::m_PixelPerIUnit property).
        int w, h;
        SDL_QueryTexture(GetTexture(), nullptr, nullptr, &w, &h);
        glm::vec2 size = glm::vec2(w, h) / glm::vec2(m_PixelsPerUnit);
        return size;
    }

    void NativeScriptComponent::Unbind() {
        if (script_instance)
            delete script_instance;
    }

    LuaScriptComponent::LuaScriptComponent() {
        lua = CreateRef<sol::state>();
        // Load standard libraries for basic functionality.
        lua->open_libraries();
    }
    void LuaScriptComponent::Attach(std::string name, std::filesystem::path script_path) {
        REN_ASSERT(scripts.count(name) == 0, "Script with name " + name + " is already attached to LuaScriptComponent.");
        scripts[name] = CreateRef<LuaScript>(name, lua.get(), script_path);
    }

    void LuaScriptComponent::Detach(std::string name) {
        REN_ASSERT(scripts.count(name) != 0, "Script with name " + name + " is not attached");
        scripts.erase(name);
    }
};
