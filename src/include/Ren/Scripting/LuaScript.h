#pragma once
#include <variant>
#include <vector>
#include <string>
#include <filesystem>
#include <any>
#include <sol/sol.hpp>

#include "Ren/ECS/Scene.h"
#include "Ren/Core/Input.hpp"

namespace Ren
{
    /// Handles communication between LUA and C++
    class LuaScript
    {
        const std::string NAME{ "undefined" };
        const std::string INSTANCE_NAME{ "i_" + NAME };     // Name of the metatable instance in LUA.
        const std::string ON_INIT{ "OnInit" };
        const std::string ON_DESTROY{ "OnDestroy" };
        const std::string ON_UPDATE{ "OnUpdate" };
    public:
        /// @param name Name of the LUA metatable. Must be unique.
        /// @param lua_state Pointer to the lua state to use.
        /// @param script_path Path to script relative to AssetManager::m_ScriptDir
        LuaScript(std::string name, sol::state* lua_state, std::filesystem::path script_path);
        ~LuaScript() {}

        const std::string& GetName() { return NAME; }
        std::string GetScriptPath() { return m_scriptPath.string(); }

        /// Call OnInit() function in LUA
        void OnInit();
        /// Call OnDestroy() function in LUA
        void OnDestroy();
        /// Call NAME:OnUpdate(dt) function in LUA
        /// @param dt Delta time
        void OnUpdate(float dt);

    private:
        sol::state*     m_lua{ nullptr };
        Entity          m_entity{};
        KeyInterface*   m_input{ nullptr };
        std::filesystem::path   m_scriptPath{};

        // Create class for this script and setup it. Should be called by LuaScriptSystem only.
        void init();
        // Create custom datatypes and stuff for this lua state.
        // FIXME: Maybe this should be set by the LuaScriptSystem instead, but I like that
        //        all the sol2 LUA stuff is here.
        void first_init();
        // Destroy class etc. Should be called by LuaScriptSystem only.
        void destroy();

        friend class LuaScriptSystem;
    };
}
