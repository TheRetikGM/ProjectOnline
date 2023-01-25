#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <any>
#include <sol/sol.hpp>
#include <optional>
#include <any>
#include <array>
#include <yaml-cpp/yaml.h>

#include "Ren/ECS/Scene.h"
#include "Ren/Core/Input.hpp"

namespace Ren
{
    // We expose only basic types for simplicity.
    enum class LuaParamType : int { number = 0, string, boolean };
    const static std::array<const char*, 3> LUA_PARAM_TYPE_S = { "number", "string", "boolean" }; 

    class LuaScript;
    /// Hold information about parameters present in given LuaScript instance.
    class LuaParam
    {
    public:
        /// Name of the parameter in LUA parameter array.
        std::string m_Name;
        /// Data type of the parameter in LUA parameter array.
        LuaParamType m_Type;
        /// Pointer to the LuaScript instsance holding this parameter.
        LuaScript* m_Script;

        /// @param p_script Pointer to the LuaScript instance holding this parameter.
        /// @param name Name of this parameter in LUA parameter table.
        /// NOTE: Type will be deduced automatically.
        LuaParam(LuaScript* p_script, const std::string& name);
        LuaParam() = default;

        /// Get value from LUA.
        template<typename T> T Get();
        /// Set value in LUA.
        template<typename T> void Set(T val);
        /// Return LUA value when casting.
        template<typename T> operator T() { return Get<T>(); }
        /// Set value in LUA.
        template<typename T> LuaParam& operator=(const T& val) { Set<T>(val); return *this; }

    private:
        std::any m_data;
        Ref<std::any> m_cachedData;

        // Set value from m_chachedData. Called from LuaScript::init().
        // It is used when deserializing. We need to store the value here so it won't be overriden by anything.
        void setFromData();

        friend class LuaScript;
        friend struct YAML::convert<Ren::LuaParam>;
    };

    /// Handles communication between LUA and C++
    class LuaScript
    {
        // Name of the table in LUA on which in which can defined following functions.
        const std::string NAME{ "undefined" };
        // Name of the parameter table inside NAME table.
        const std::string PARAM{ "p" };
        const std::string ON_INIT{ "OnInit" };
        const std::string ON_DESTROY{ "OnDestroy" };
        const std::string ON_UPDATE{ "OnUpdate" };
    public:
        std::vector<LuaParam> m_Parameters;

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
        friend class LuaParam;
    };
}
