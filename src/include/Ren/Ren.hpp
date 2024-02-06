/**
 * @file Ren/Ren.hpp
 * @brief Include everything with one file. This is just for ease of use for users.
 */

#pragma once
#include "config.h"


extern "C" {
    #include <SDL.h>
    #include <SDL_image.h>
}
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <ren_utils/utils.h>

#include "Ren/Core/Core.hpp"
#include "Ren/Core/Startup.hpp"
#include "Ren/Core/GameCore.hpp"
#include "Ren/Core/Layer.hpp"
#include "Ren/Core/AssetManager.hpp"

#include "ECS/Scene.hpp"
#include "Scripting/NativeScript.hpp"
#include "ECS/Serialization/SceneSerializer.hpp"

#include "Utils/FpsCounter.hpp"

#include "Renderer/Renderer.hpp"
#include "Renderer/TextRenderer.hpp"

#include "Ren/Physics/Physics.hpp"
