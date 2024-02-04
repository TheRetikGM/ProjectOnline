#pragma once
#include <Ren/Ren.h>

/// Manage scene in editor.
class Scene {
    // Defines the rate of change of camera PPU on zoom.
    const glm::ivec2 ZOOM_SENSITIVITY{ 10 };
public:
    Ren::Texture2D m_SceneTexture;
    /// Default Pixels-Per-Unit for loaded scene. Acts as a zoom level.
    glm::ivec2 m_DefaultPPU{ 50 };
    /// Default camera position for newly loaded scene.
    glm::vec2 m_DefaultCamPos{ 0.0f, 0.0f };
    /// Screen position of this Scene window.
    /// NOTE: This is required to correctly handle mouse events.
    glm::vec2 m_ScreenPos{ 0.0f, 0.0f };
    /// Disable/Enable input for this scene.
    bool m_AcceptInput = true;
    /// Edit mode means it will not be updated and you can select entities (TODO).
    bool m_EditMode = true;

    Scene(glm::ivec2 initial_size, SDL_Renderer* sdl_renderer, Ren::KeyInterface* key_interface);

    /// Load scene
    /// @param scene_path Path to scene relative to scene_path
    void Load(std::filesystem::path scene_path);
    /// Unload currently loaded scene without saving.
    void Unload();
    /// Reset camera position and PPU to default values.
    inline void ResetCamera() { m_camera.SetUnitScale(m_DefaultPPU); m_camera.m_CamPos = m_DefaultCamPos; }

    /// Process input. All input handling should be implemeneted here, as we want
    /// to be able to disable it if needed.
    void ProcessInput(float dt);
    /// Process mouse wheel event.
    void ProcessMouseWheel(int wheel_y_offset);
    /// Update scene.
    void Update(float dt);
    /// Render scene to m_SceneTexture.
    void Render();
    /// Resize scene to given size.
    void Resize(glm::ivec2 new_size);
    /// Serialize the scene to the same file it was loaded from.
    void Save();
    /// Serialize the scene.
    /// @param save_path Path relative to the AssetManager::GetSceneDir()
    void SaveAs(std::filesystem::path path);
    /// Enable debug mode for scene subsystems.
    void SetDebug(bool enable);

    inline glm::ivec2 GetSize() { return m_SceneTexture.m_Size; }
    /// Is there some scene loaded?
    inline bool GetLoadState() { return m_loadedState; }
    /// Return Ren::Scene object for direct scene management.
    inline Ren::Scene& Get() { return *m_scene; }

private:
    Ref<Ren::Scene> m_scene;
    // Camera used in the scene.
    Ren::CartesianCamera m_camera;
    // Path to currently loaded scene.
    std::filesystem::path m_loadedScene;
    // Save loaded state for querying.
    bool m_loadedState  = false;

    // Used for scene deserialization.
    SDL_Renderer* m_sdlRenderer;
    Ren::KeyInterface* m_keyInterface;
};
