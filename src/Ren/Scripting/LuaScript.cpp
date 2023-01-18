#include "Ren/Scripting/LuaScript.h"
#include "Ren/Utils/Logger.hpp"
#include "sol/types.hpp"
#include "Ren/Core/Core.h"
#include "Ren/Core/AssetManager.hpp"

using namespace Ren;

struct LuaInterface
{
    int num = 0;
    LuaScript* host = nullptr;

    LuaInterface(LuaScript* host, KeyInterface* input) : num(0), host(host) {}
    LuaInterface() {}

    static bool KeyPressed(KeyInterface* p_input, int key) { return p_input->KeyPressed(Key(key)); }
    static bool KeyHeld(KeyInterface* p_input, int key) {    return p_input->KeyHeld(Key(key)); }
};

LuaScript::LuaScript(std::string name, sol::state* lua_state, std::filesystem::path script_path)
    : NAME(name)
    , m_lua(lua_state)
    , m_scriptPath(script_path)
{}

void LuaScript::init()
{
    REN_ASSERT(m_input != nullptr, "Did you call this from LuaScriptSystem::InitScript?");
    REN_ASSERT(m_entity.id != Entity{}.id, "Entity must be set.");

    // Check if this is first init on this component.
    auto lua_int_set = (*m_lua)["_lua_int_set"];
    if (!lua_int_set.valid())
        first_init();

    // Create metatable instance for this script.
    m_lua->set(INSTANCE_NAME, LuaInterface(this, m_input));
    m_lua->script_file(m_scriptPath);
}

void LuaScript::first_init()
{
    m_lua->set("_lua_int_set", true);

    // Set functions provided for all scripts on this component.
    // FIXME: Should we implement custom KeyInterface and only update it when key is pressed?
    m_lua->set("REN_INPUT", m_input);
    m_lua->set_function("API_KeyPressed", LuaInterface::KeyPressed);
    m_lua->set_function("API_KeyHeld", LuaInterface::KeyHeld);

    // Create custom class for LUA with this script specific properties.
    auto lua_type = m_lua->new_usertype<LuaInterface>(NAME, sol::constructors<LuaInterface(LuaScript*, KeyInterface*), LuaInterface()>());
    lua_type["num"] = &LuaInterface::num;
    lua_type["host"] = &LuaInterface::host;

    m_lua->set("LUA_PATH", "?;?.lua;" 
            + AssetManager::GetLuaCoreDir().string() + "/?.lua;"
            + AssetManager::GetLuaCoreDir().string() + "/?;");

    m_lua->script_file(AssetManager::GetLuaCore("keys.lua"));
    m_lua->script_file(AssetManager::GetLuaCore("input.lua"));
}

void LuaScript::destroy()
{
    // For now there is nothing to do.
    // There we could free up any resources and stuff that we created in LuaScript::init()
}

// Call "method" on metatable named 'instance_name'.
template<typename... Args>
inline void lua_method(sol::state* lua, const std::string& instance_name, const std::string& method_name, Args... args)
{
    sol::table t = (*lua)[instance_name];
    auto method = t[method_name];
    if (method.valid())
        method.call(t, args...);
}

void LuaScript::OnInit() {           lua_method(m_lua, INSTANCE_NAME, ON_INIT); }
void LuaScript::OnDestroy() {        lua_method(m_lua, INSTANCE_NAME, ON_DESTROY); }
void LuaScript::OnUpdate(float dt) { lua_method(m_lua, INSTANCE_NAME, ON_UPDATE, dt); }
