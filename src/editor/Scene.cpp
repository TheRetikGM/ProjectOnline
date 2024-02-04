/**
 * @file Ren/ECS/Scene.cpp
 * @brief Implementation of scene object.
 */

#include <ren_utils/logging.hpp>

#include "Scene.h"

Scene::Scene(glm::ivec2 initial_size, SDL_Renderer* sdl_renderer, Ren::KeyInterface* key_interface)
    : m_sdlRenderer(sdl_renderer)
    , m_keyInterface(key_interface)
{
    m_SceneTexture.m_Size = initial_size;
    m_SceneTexture.m_Format = SDL_PIXELFORMAT_RGBA32;
    m_SceneTexture.m_Access = SDL_TEXTUREACCESS_TARGET;
    m_SceneTexture.Generate();
}

void Scene::Load(std::filesystem::path scene_path) {
    // Discard the current scene if loaded.
    Unload();

    try {
        m_scene = Ren::SceneSerializer::Deserialze(scene_path, m_sdlRenderer, m_keyInterface);
        m_scene->Init();

        m_camera.m_CamPos = m_DefaultCamPos;
        m_camera.SetUnitScale(m_DefaultPPU);

        m_loadedState = true;
    } catch (std::runtime_error& e) {
        LOG_E("Failed to load scene. Error: " + std::string(e.what()));
    }
}
void Scene::Unload() {
    if (m_scene)
        m_scene.reset();
    m_loadedState = false;
    m_SceneTexture.Generate(); // Clear texture.
    m_EditMode = true;
}

void Scene::ProcessInput(float dt) {
    if (!m_AcceptInput)
        return;

    // Move camera if right button is pressed.
    int x, y;
    bool mouse_pressed = SDL_GetRelativeMouseState(&x, &y) & SDL_BUTTON_RMASK;
    if (mouse_pressed)
        m_camera.m_CamPos -= glm::vec2(x, -y) / glm::vec2(m_camera.GetUnitScale());
}
void Scene::ProcessMouseWheel(int wheel_y_offset) {
    if (!m_AcceptInput)
        return;

    m_camera.SetUnitScale(m_camera.GetUnitScale() + wheel_y_offset * ZOOM_SENSITIVITY);
}
void Scene::Update(float dt) {
    if (!m_EditMode)
        m_scene->Update(dt);
}
void Scene::Render() {
    Ren::Renderer::BeginRender(&m_camera, &m_SceneTexture);
    Ren::Renderer::Clear({ 20, 20, 20, 255 });
    m_scene->Render();

    if (m_AcceptInput) {
        int x, y;
        bool mouse_pressed = SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK;
        if (mouse_pressed) {
            glm::vec2 mouse_pos = m_camera.ToUnits(glm::vec2( x, y ) - m_ScreenPos);
            Ren::Renderer::DrawCircle(mouse_pos, 0.2f, Ren::Colors4::Yellow);
        }
    }

    Ren::Renderer::Render();
    Ren::Renderer::EndRender();

}
void Scene::Resize(glm::ivec2 new_size) {
    if (new_size.x <= 0 || new_size.y <= 0) {
        LOG_E("Cannot resize scene to size { " + std::to_string(new_size.x) + ", " + std::to_string(new_size.y) + " }.");
        return;
    }

    m_camera.SetViewportSize(new_size);
    m_SceneTexture.m_Size = new_size;
    m_SceneTexture.Generate();
}
void Scene::Save() {
    try {
        if (!m_scene)
            return;
        Ren::SceneSerializer::Serialize(m_scene, Ren::AssetManager::GetScene(m_loadedScene));
    } catch (const std::runtime_error& e) {
        LOG_E("Failed to save scene. Error: " + std::string(e.what()));
    }
}
void Scene::SaveAs(std::filesystem::path path) {
    try {
        if (!m_scene)
            return;
        Ren::SceneSerializer::Serialize(m_scene, Ren::AssetManager::GetScene(path));
    } catch (const std::runtime_error& e) {
        LOG_E("Failed to save scene as '" + path.string() + "'. Error: " + std::string(e.what()));
    }
}
void Scene::SetDebug(bool enable) {
    if (!m_loadedState) {
        LOG_E("Scene is not loaded. Cannot set debug mode.");
        return;
    }
    m_scene->GetSystem<Ren::PhysicsSystem>()->m_DebugRender = enable;
}
