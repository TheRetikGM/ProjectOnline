#include <exception>
#include <stdexcept>

#include "Ren/Scripting/LuaScript.h"
#include "Ren/Utils/Logger.hpp"
#include "Ren/Core/Core.h"
#include "Ren/Core/AssetManager.hpp"
#include "Ren/ECS/Components.h"

#include "sol/types.hpp"
#include "sol/property.hpp"

using namespace Ren;

struct LuaInterface
{
    static bool KeyPressed(KeyInterface* p_input, int key) { return p_input->KeyPressed(Key(key)); }
    static bool KeyHeld(KeyInterface* p_input, int key) {    return p_input->KeyHeld(Key(key)); }
    static void Log(int level, std::string message, std::string file, int line)
    {
        // TODO: Maybe somehow tag that this is from LUA?
        if (level >= 0 && level <= 4)
            LogEmmiter::Log(LogLevel(level), message, file, line);
    }
};

class UnsupportedTypeException : public std::exception
{
    std::string type_name;
public:
    UnsupportedTypeException(std::string type_name) : type_name(type_name) {}
    const char* what() const noexcept override { return type_name.c_str(); }
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

    // Check if table already exists.
    auto table_check = (*m_lua)[NAME];
    REN_ASSERT(!table_check.valid(), "LuaScript with given name is already bound to this entity.");

    // Create metatable instance for this script.
    (*m_lua)[NAME] = m_lua->create_table_with( 
            "host", this,
            PARAM, m_lua->create_table_with(),
            "API_RegParam", [this](const std::string& name){ 
                try {
                    if (std::find_if(m_Parameters.begin(), m_Parameters.end(), [&name](const auto& p) { return p.m_Name == name; }) != m_Parameters.end())
                        return;
                    m_Parameters.push_back(LuaParam(this, name));
                } catch (const UnsupportedTypeException& e) {
                    LOG_W("Script '" + NAME + "' has a parameter '" + name + "' of unsupported type '" + e.what() + "'. Supported types are number, string and boolean.");
                    return;
                }
            });

    // Setup NAME.PARAM array to automatically inform C++ about new parameters. Defined in core.lua
    (*m_lua)["C_SetupParam"](NAME, PARAM);

    // Execute script file provided.
    m_lua->script_file(AssetManager::GetScript(m_scriptPath).string());
    
    // Set default values for parameters
    for (auto i : m_Parameters)
        i.setFromData();
}

void LuaScript::first_init()
{
    m_lua->set("_lua_int_set", true);

    // Set functions provided for all scripts on this component.
    // FIXME: Should we implement custom KeyInterface and only update it when key is pressed?
    m_lua->set("REN_INPUT", m_input);
    m_lua->set_function("API_KeyPressed", LuaInterface::KeyPressed);
    m_lua->set_function("API_KeyHeld", LuaInterface::KeyHeld);
    m_lua->set_function("API_Log", LuaInterface::Log);

    // Create transform component type.
    auto lua_vec2 = m_lua->new_usertype<glm::vec2>("glmvec2", sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y,
            "size", sol::property(&glm::vec2::length));
    auto lua_ivec2 = m_lua->new_usertype<glm::ivec2>("glmivec2", sol::constructors<glm::ivec2(), glm::ivec2(int), glm::ivec2(int, int)>(),
            "x", &glm::ivec2::x,
            "y", &glm::ivec2::y,
            "size", sol::property(&glm::ivec2::length));
    auto lua_tr = m_lua->new_usertype<TransformComponent>("TransformComponent", sol::constructors<TransformComponent()>(),
            "position", &Ren::TransformComponent::position,
            "scale", &Ren::TransformComponent::scale,
            "rotation", &Ren::TransformComponent::rotation,
            "layer", &Ren::TransformComponent::layer,
            "dirty", &Ren::TransformComponent::dirty);
    auto lua_sc = m_lua->new_usertype<SpriteComponent>("SpriteComponent", sol::constructors<SpriteComponent()>(),
            "color", &SpriteComponent::m_Color,
            "ppu", &SpriteComponent::m_PixelsPerUnit);

    m_lua->set("LUA_PATH", "?;?.lua;" 
            + AssetManager::GetLuaCoreDir().string() + "/?.lua;"
            + AssetManager::GetLuaCoreDir().string() + "/?;"
            + AssetManager::GetScriptDir().string() + "/?.lua;"
            + AssetManager::GetScriptDir().string() + "/?;");

    // Create component references in lua
    (*m_lua)["Components"] = m_lua->create_table_with(
        "Transform", std::ref(m_entity.Get<TransformComponent>()),
        "Sprite", std::ref(m_entity.Get<SpriteComponent>())
    );

    m_lua->script_file(AssetManager::GetLuaCore("core.lua").string());
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

void LuaScript::OnInit() {           lua_method(m_lua, NAME, ON_INIT); }
void LuaScript::OnDestroy() {        lua_method(m_lua, NAME, ON_DESTROY); }
void LuaScript::OnUpdate(float dt) { lua_method(m_lua, NAME, ON_UPDATE, dt); }

LuaParam::LuaParam(LuaScript* p_script, const std::string& name)
    : m_Name(name)
    , m_Script(p_script)
{
    std::string s = "_lua_temp = type(" + m_Script->NAME + "." + m_Script->PARAM + "." + name + ")";
    m_Script->m_lua->script(s);
    std::string type = m_Script->m_lua->get<std::string>("_lua_temp");

    // Parse the type info enumartion value.
    if (type == "number") {
        m_Type = LuaParamType::number;
        Get<float>();
    }
    else if (type == "string") {
        m_Type = LuaParamType::string;
        Get<std::string>();
    }
    else if (type == "boolean") {
        m_Type = LuaParamType::boolean;
        Get<bool>();
    }
    else
        throw UnsupportedTypeException(type);
}

void LuaParam::setFromData()
{
    if (!m_cachedData)
        return;

    switch (m_Type) {
        case LuaParamType::number: Set(std::any_cast<float>(*m_cachedData)); break;
        case LuaParamType::string: Set(std::any_cast<std::string>(*m_cachedData)); break;
        case LuaParamType::boolean: Set(std::any_cast<bool>(*m_cachedData)); break;
    }
}

template<typename T> T LuaParam::Get()
{
    T val = (*m_Script->m_lua)[m_Script->NAME][m_Script->PARAM][m_Name];
    m_data = val;
    return val;
}

template<typename T> void LuaParam::Set(T val)
{
    (*m_Script->m_lua)[m_Script->NAME][m_Script->PARAM][m_Name] = val;
}

template<> int LuaParam::Get<int>() { return (int)Get<float>(); }
template float LuaParam::Get<float>();
template std::string LuaParam::Get<std::string>();
template bool LuaParam::Get<bool>();
template<> void LuaParam::Set<int>(int v) { Set((float)v); }
template void LuaParam::Set<float>(float);
template void LuaParam::Set<std::string>(std::string);
template<> void LuaParam::Set<const char*>(const char* value) { Set(std::string(value)); }
template void LuaParam::Set<bool>(bool);
