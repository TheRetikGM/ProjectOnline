#include <tuple>
#include <vector>
#include <algorithm>    // std::find

#include "Ren/ECS/Scene.h"
#include "Ren/Utils/Logger.hpp"

namespace Ren
{
    Scene::Scene(SDL_Renderer* renderer, KeyInterface* input)
        : m_input(input)
    {
        REN_ASSERT(renderer != nullptr, "Invalid renderer.");
        m_Renderer = renderer;

        // Create entt scene
        if (!m_Registry)
            m_Registry = CreateRef<entt::registry>();
        else
            m_Registry->clear();

        // Create texture cache used for storing loaded textures.
        if (!m_textureCache)
            m_textureCache = CreateRef<TextureCache>();
        else
            m_textureCache->clear();

        // Automatically load textures on construct. If path is not provided, then the texture has to be loaded manually with Scene::LoadTexture().
        m_Registry->on_construct<ImgComponent>().connect<&Scene::onTextureConstruct<ImgComponent>>(this);
        m_Registry->on_construct<SpriteComponent>().connect<&Scene::onTextureConstruct<SpriteComponent>>(this);

        // Add common systems.
        AddSystem<RenderSystem>();
        AddSystem<NativeScriptSystem>();
        AddSystem<LuaScriptSystem>();
        AddSystem<PhysicsSystem>();
    }
    Scene::~Scene()
    {
        m_sysManager.Clear();
        m_textureCache->clear();
        m_Registry->clear();
    }

    Entity Scene::CreateEntity(const TransformComponent& transform_comp, const TagList& tag_list)
    {
        Entity ent{ m_Registry->create(), this };
        ent.Add<TransformComponent>() = transform_comp;
        for (auto&& tag : tag_list)
            AddTag(ent, tag);
        return ent;
    };
    void Scene::AddTag(Entity ent, std::string tag)
    {
        if (HasTag(ent, tag))
            return;
        m_entityToTags[ent.id].push_back(tag);
        m_tagToEntities[tag].push_back(ent.id);
    }
    bool Scene::HasTag(Entity ent, std::string tag)
    {
        return std::find(m_entityToTags[ent.id].begin(), m_entityToTags[ent.id].end(), tag) != m_entityToTags[ent.id].end();
    }
    void Scene::RemTag(Entity ent, std::string tag)
    {
        auto tag_iter = std::find(m_entityToTags[ent.id].begin(), m_entityToTags[ent.id].end(), tag);
        if (tag_iter == m_entityToTags[ent.id].end())
            return;
        auto ent_iter = std::find(m_tagToEntities[tag].begin(), m_tagToEntities[tag].end(), ent.id);

        m_entityToTags[ent.id].erase(tag_iter);
        m_tagToEntities[tag].erase(ent_iter);
    }
    Ref<std::vector<Entity>> Scene::GetEntitiesByTag(const std::string& tag)
    {
        auto ent_arr = CreateRef<std::vector<Entity>>();
        
        // Check if tag exists at all, so we dont create empty std::list<entt::entity> when using [] operator.
        if (m_tagToEntities.count(tag) == 0)
            return ent_arr;
        
        std::list<entt::entity>& found_entities = m_tagToEntities[tag];
        for (auto&& ent : found_entities)
            ent_arr->push_back({ ent, this });

        return ent_arr;
    }

    std::tuple<bool, Entity> Scene::GetEntityByTag(const std::string& tag)
    {
        // Check if tag exists at all, so we dont create empty std::list<entt::entity> when using [] operator.
        if (m_tagToEntities.count(tag) == 0)
            return { false, {} };

        auto& found_entities = m_tagToEntities[tag];
        return { found_entities.size() > 0, { found_entities.front(), this } };
    }
    
    void Scene::LoadTexture(ImgComponent* component)
    {
        auto [success, handle] = LoadTexture(component->img_path);
        if (success)
            component->texture_handle = handle;
    }
    std::tuple<bool, TextureHandle> Scene::LoadTexture(std::filesystem::path path)
    {
        // Load the component, only if the path is specified.
        // Note that the texture cache load will be unsuccessfull only if the texture is already loaded **NOT** when the load
        //  of texture itself is unsuccessfull (when that happens, there is undefined behavior). Those cases are already
        //  accounted for with asserts in the loader.
        if (path != UNDEFINED_PATH)
        {
            auto ret = m_textureCache->load(entt::hashed_string(path.string().c_str()), m_Renderer, path.string().c_str());
            return std::make_tuple(true, *ret.first);
        }
        else
            LOG_W("Trying to load image with unspecified path.");
        return std::make_tuple(false, TextureHandle{});
    }
} // namespace Ren
