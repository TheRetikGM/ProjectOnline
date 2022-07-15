#pragma once
#include "Ren/Core.h"
#include "Components.h"
#include "Loaders.hpp"
#include <entt/entt.hpp>

namespace Ren
{
    class Scene
    {
    public:
        // Wrapper for EnTT entity. It is used for cleaner code and easier acces to the scene.
        //     - Note: Has to be inside scene, because it references scene (and scene references entity) with inline definitions. We could 
        //             also move the definitions outside of the struct, but that would be pain with templates.
        struct Entity
        {
            entt::entity id{ entt::null };
            Scene* p_scene{ nullptr };

            Entity() = default;
            Entity(entt::entity ent_id, Scene* p_scene) : id(ent_id), p_scene(p_scene) {}

            // Add given component. Args are arguments needed for construction.
            template<typename TComponent, typename... Args>
            inline auto& Add(Args&&... args)
            {
                return p_scene->m_Registry->emplace<TComponent>(id, std::forward<Args>(args)...);
            }

            // Get given components.
            template<typename... TComponents>
            inline auto& Get()
            {
                return p_scene->m_Registry->get<TComponents...>(id);
            }

            // Remove given components.
            template<typename... TComponents>
            inline auto& Remove()
            {
                p_scene->m_Registry->remove<TComponents...>(id);
            }

            // Returns true if entity has ALL of given components.
            template<typename... TComponents>
            inline bool HasAll()
            {
                return p_scene->m_Registry->all_of<TComponents...>(id);
            }

            // Returns true, if entity has ANY of the given components.
            template<typename... TComponents>
            inline bool HasAny()
            {
                return p_scene->m_Registry->any_of<TComponents...>(id);
            }
        };
    public:
        Ref<entt::registry> m_Registry{ nullptr };
        // Used for loading textures.
        SDL_Renderer* m_Renderer{ nullptr };

        Scene() = default;

        // Initializes empty scene.
        inline void Init(SDL_Renderer* renderer)
        {
            REN_ASSERT(renderer != nullptr, "Invalid renderer.");
            m_Renderer = renderer;

            m_Registry = CreateRef<entt::registry>();
            m_textureCache = CreateRef<TextureCache>();

            // Automatically load textures on construct. If path is not provided, then the texture has to be loaded manually with Scene::LoadTexture().
            m_Registry->on_construct<SpriteComponent>().connect<&Scene::onTextureConstruct<SpriteComponent>>(this);
        }
        // Removes all entities and their components.
        inline void Destroy()
        {
            m_Registry->clear();
            m_textureCache->clear();
        }

        // Create entity with default components.
        inline Entity CreateEntity(const TransformComponent& transform_comp = {}, const TagComponent& tag_comp = {})
        {
            Entity ent{ m_Registry->create(), this };
            ent.Add<TransformComponent>() = transform_comp;
            ent.Add<TagComponent>() = tag_comp;
            return ent;
        };

        // Destroy given entity.
        inline void DestroyEntity(Entity ent)
        {
            m_Registry->destroy(ent.id);
        }

        // Checks if given entity is still valid (e.g. it was not destroyed).
        inline bool EntityValid(Entity ent) const
        {
            return m_Registry->valid(ent.id);
        }
        
        // Get all entities with given tag.
        Ref<std::vector<Entity>> GetEntitiesByTag(const std::string& tag)
        {
            auto view = m_Registry->view<TagComponent>();
            auto matches = CreateRef<std::vector<Entity>>();
            for (auto&& ent : view)
                if (view.get<TagComponent>(ent).tag == tag)
                    matches->push_back({ ent, this });
            return matches;
        }

        // Get the first entity with given tag. First tuple element represents success status.
        std::tuple<bool, Entity> GetEntityByTag(const std::string& tag)
        {
            auto view = m_Registry->view<TagComponent>();
            
            for (auto&& ent : view)
                if (view.get<TagComponent>(ent).tag == tag)
                    return std::make_tuple(true, Entity{ ent, this });

            return std::make_tuple(false, Entity{});
        }

        // TODO: resource loading, events, callbacks etc.

        // Loads texture for given component reference. 
        void LoadTexture(ImgComponent* component)
        {
            if (component->img_path != UNDEFINED_PATH)
            {
                auto ret = m_textureCache->load(entt::hashed_string(component->img_path.c_str()), m_Renderer, component->img_path.c_str());
                component->texture_handle = *ret.first;
            }
        }

        Ref<TextureCache> GetTextureCache() { return m_textureCache; }

    private:
        // Cache for storing loaded textures of components.
        Ref<TextureCache> m_textureCache{ nullptr };

        template<typename T>
        inline void onTextureConstruct(entt::registry& reg, entt::entity ent)
        {
            LoadTexture(dynamic_cast<ImgComponent*>(&reg.get<T>(ent)));
        }
    };

    using Entity = Scene::Entity;
};