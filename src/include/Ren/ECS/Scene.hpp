/**
 * @file Ren/ECS/Scene.hpp
 * @brief Declaration of scene object.
 */
#pragma once
#include <entt/entt.hpp>
#include <filesystem> // std::filesystem::path
#include <optional>

#include "Ren/Core/Core.hpp"
#include "Components.hpp"
#include "Loaders.hpp"
#include "ComponentSystems.hpp"
#include "SystemsManager.hpp"
#include "Ren/Physics/Physics.hpp"

namespace Ren {
    using TagList = std::list<std::string>;

    /*
        Does several things:
            - Wraps EnTT's registry and add some useful functions for our engine use (such as UUIDs (TODO))
            - Holds systems manager.
            - Holds component resources (loaded textures, ...)
    */
    class Scene {
    public:
        /// Wrapper for EnTT entity. It is used for cleaner code and easier acces to the scene.
        ///     - Note: Has to be inside scene, because it references scene (and scene references entity) with inline definitions. We could
        ///             also move the definitions outside of the struct, but that would be pain with templates.
        struct Entity {
            entt::entity id{ entt::null };
            Scene* p_scene{ nullptr };

            Entity() = default;
            Entity(entt::entity ent_id, Scene* p_scene) : id(ent_id), p_scene(p_scene) {}

            /// Add given component. Args are arguments needed for construction.
            template<typename TComponent, typename... Args>
            inline auto& Add(Args&&... args) { return p_scene->m_Registry->emplace<TComponent>(id, std::forward<Args>(args)...); }

            /// Get given components.
            template<typename TComponent>
            inline auto& Get() { return p_scene->m_Registry->get<TComponent>(id); }
            template<typename... TComponents>
            inline auto GetM() { return p_scene->m_Registry->get<TComponents...>(id); }

            /// Remove given components.
            template<typename... TComponents>
            inline auto& Remove() { p_scene->m_Registry->remove<TComponents...>(id); }

            /// Returns true if entity has ALL of given components.
            template<typename... TComponents>
            inline bool HasAll() { return p_scene->m_Registry->all_of<TComponents...>(id); }

            /// Returns true, if entity has ANY of the given components.
            template<typename... TComponents>
            inline bool HasAny() { return p_scene->m_Registry->any_of<TComponents...>(id); }

            void AddTag(std::string tag) { p_scene->AddTag(*this, tag); };
            bool HasTag(std::string tag) { return p_scene->HasTag(*this, tag); };
            void RemTag(std::string tag) { p_scene->RemTag(*this, tag); };
            const TagList& GetTags() { return p_scene->m_entityToTags[id]; }

            // Override cast operator for less painfull converting.
            operator entt::entity() { return this->id; }
        };  // struct Entity
    public:
        Ref<entt::registry> m_Registry{ nullptr };
        // Used for loading textures.
        SDL_Renderer* m_Renderer{ nullptr };
        /// Name of the scene.
        std::string m_Name = "Undefined";

        Scene(SDL_Renderer* renderer, KeyInterface* input);
        ~Scene();

        /// Create entity with default components.
        Entity CreateEntity(const TransformComponent& transform_comp = {}, const TagList& tag_list = {});
        /// Create Ren::Entity from entt::entity.
        inline Entity ToEntity(entt::entity ent) { return { ent, this }; }
        /// Destroy given entity.
        inline void DestroyEntity(Entity ent) { m_Registry->destroy(ent.id); }
        /// Checks if given entity is still valid (e.g. it was not destroyed).
        inline bool EntityValid(Entity ent) const { return m_Registry->valid(ent.id); }

        /// Shortcut to EnTT registry view.
        template<typename... TComponents>
        inline auto SceneView() { return m_Registry->view<TComponents...>(); }

        /// Get all entities with given tag.
        Ref<std::vector<Entity>> GetEntitiesByTag(const std::string& tag);
        /// Get the first entity with given tag.
        std::optional<Entity> GetEntityByTag(const std::string& tag);

        void AddTag(Entity ent, std::string tag);
        bool HasTag(Entity ent, std::string tag);
        void RemTag(Entity ent, std::string tag);
        inline void AddTag(entt::entity ent, std::string tag) { AddTag({ ent, this }, tag); }
        inline bool HasTag(entt::entity ent, std::string tag) { return HasTag({ ent, this }, tag); }
        inline void RemTag(entt::entity ent, std::string tag) { RemTag({ ent, this }, tag); }

        /// Loads texture for given component reference.
        void LoadTexture(ImgComponent* component);
        /// Load texture with given, path. If texture already exists in cache, then it is recycled.
        /// @returns [sucess, texture_handle]
        std::optional<TextureHandle> LoadTexture(std::filesystem::path path);
        /// Returns raw pointer to the texture cache.
        inline TextureCache* GetTextureCache() { return m_textureCache.get(); }

        /* Systems manager forwarders */

        /// Add component system. Note that the type must inherit from ComponentSystem base class.
        template<typename T, typename... Args>
        inline T* AddSystem(Args... args) { return m_sysManager.Add<T>(this, m_input, std::forward<Args>(args)...); }
        /// Remove component system.
        template<typename T>
        inline void RemoveSystem() { m_sysManager.Remove<T>(); }
        /// Return pointer to the component system.
        template<typename T>
        inline T* GetSystem() { return m_sysManager.Get<T>(); }

        /// Initializes all subystems of the scene.
        inline void Init() { m_sysManager.Init(); }
        /// Call destroy on all component systems.
        inline void Destroy() { m_sysManager.Destroy(); }
        /// Call update on all component systems.
        /// @param dt Delta time
        inline void Update(float dt) { m_sysManager.Update(dt); }
        /// Call render on all component systems.
        inline void Render() { m_sysManager.Render(); }

    private:
        // Cache for storing loaded textures of components.
        Ref<TextureCache> m_textureCache{ nullptr };
        // Stores and manages all systems in scene.
        SystemsManager m_sysManager;
        // Used for auto passing as argument to systems.
        KeyInterface* m_input;
        // Map tag to entities to speed up the search time for entities by tag.
        std::unordered_map<std::string, std::list<entt::entity>> m_tagToEntities{};
        // Map entity to tags to speed up tag searchup.
        std::unordered_map<entt::entity, std::list<std::string>> m_entityToTags{};

        // Load texture (into texture cache) when the component is constructed. It will be unloaded when the cache is cleared.
        template<typename T>
        inline void onTextureConstruct(entt::registry& reg, entt::entity ent) { LoadTexture(dynamic_cast<ImgComponent*>(&reg.get<T>(ent))); }

        friend class SceneSerializer;
    }; // class Scene

    using Entity = Scene::Entity;
}; // namespace Ren
