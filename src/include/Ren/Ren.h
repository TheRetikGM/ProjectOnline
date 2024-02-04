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

#include "Ren/Core/Core.h"
#include "Ren/Core/Startup.h"
#include "Ren/Core/GameCore.h"
#include "Ren/Core/Layer.h"
#include "Ren/Core/AssetManager.hpp"

#include "ECS/Scene.h"
#include "Scripting/NativeScript.h"
#include "ECS/Serialization/SceneSerializer.hpp"

#include "Utils/FpsCounter.hpp"

#include "Renderer/Renderer.h"
#include "Renderer/TextRenderer.h"

#include "Ren/Physics/Physics.h"
