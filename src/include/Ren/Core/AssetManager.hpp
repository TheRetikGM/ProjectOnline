#pragma once
#include <filesystem>
#include <string>
#include "config.h"

namespace Ren {
    using Path = std::filesystem::path;

    /// Manage asset paths.
    class AssetManager {
    public:
        inline static Path m_AssetRoot{ Path(SOURCE_DIR) / "assets" };
        inline static Path m_ScriptDir{ "scripts" };
        inline static Path m_FontDir  { "fonts" };
        inline static Path m_ScenePath{ "scenes" };
        inline static Path m_ImagePath{ "images" };
        inline static Path m_LuaCorePath{ "lua_core" };

        /// Get full asset path.
        /// @param rel_path Path relative to asset root.
        static Path Get(Path rel_path) { return m_AssetRoot / rel_path; }

        /// Get full script path.
        /// @param script_path Script path relative to m_ScriptDir
        static Path GetScript(Path script_path) { return m_AssetRoot / m_ScriptDir / script_path; }
        /// Get full script directory path.
        static Path GetScriptDir() { return m_AssetRoot / m_ScriptDir; }

        /// Get full font path.
        /// @param font_path Font path relative to m_FontDir
        static Path GetFont(Path font_path) { return m_AssetRoot / m_FontDir / font_path; }
        /// Get full font directory path.
        static Path GetFontDir() { return m_AssetRoot / m_FontDir; }

        /// Get full scene path
        /// @param scene_path Scene path relative to m_SceneDir
        static Path GetScene(Path scene_path) { return m_AssetRoot / m_ScenePath / scene_path; }
        /// Get full scene directory path
        static Path GetSceneDir() { return m_AssetRoot / m_ScenePath; }

        /// Get full image path
        /// @param scene_path Image path relative to m_ImageDir
        static Path GetImage(Path image_path) { return m_AssetRoot / m_ImagePath / image_path; }
        /// Get full image directory path
        static Path GetImageDir() { return m_AssetRoot / m_ImagePath; }

        /// Get full path to core lua script.
        /// @param core_path Core script path relative to m_LuaCorePath
        static Path GetLuaCore(Path core_path) { return m_AssetRoot / m_LuaCorePath / core_path; }
        /// Get full directory path to core lua scripts.
        static Path GetLuaCoreDir() { return m_AssetRoot / m_LuaCorePath; }
    };
}

