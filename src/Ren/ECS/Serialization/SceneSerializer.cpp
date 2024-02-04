/**
 * @file Ren/ECS/Serialization/SceneSerializer.cpp
 * @brief Implementation of scene serialization
 */
#include "Ren/ECS/Serialization/SceneSerializer.hpp"
#include "Ren/Core/AssetManager.hpp"

using namespace Ren;


void SceneSerializer::Serialize(Ref<Scene> scene, std::filesystem::path path) {
    YAML::Node node;

    node["SceneName"] = scene->m_Name;

    scene->m_Registry->each([&](auto ent) {
        Entity e{ ent, scene.get() };
        YAML::Node ent_info;
        ent_info = EntitySerializer::Serialize(e);

        node["Entities"].push_back(ent_info);
    });

    std::ofstream file(path);
    file << node;
    file.close();
}

Ref<Scene> SceneSerializer::Deserialze(std::filesystem::path path, SDL_Renderer* renderer, KeyInterface* input) {
    YAML::Node node = YAML::LoadFile(AssetManager::GetScene(path).string());

    Ref<Scene> scene = CreateRef<Scene>(renderer, input);

    scene->m_Name = node["SceneName"].as<std::string>();
    for (auto&& ent : node["Entities"]) {
        Entity e = scene->CreateEntity();
        // Add components to entity.
        EntitySerializer::Deserialize(ent, e);
    }

    return scene;
}

