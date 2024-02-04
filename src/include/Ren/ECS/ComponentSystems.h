/**
 * @file Ren/ECS/ComponentSystem.hpp
 * @brief Declaration of all conponent systems.
 *
 * Component systems are objects that handle instances of some specific component types. For example all SpriteComponents.
 */
#pragma once
extern "C" {
    #include <SDL.h>
}
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <optional>

#include "Ren/Core/Core.h"
#include "Ren/Core/Input.h"

namespace Ren {
    class Scene;

    /*
        Base class for all component systems.
        - Component systems manages groups of components. For ex. render system will act on Sprite components and render them.
        NOTE: When creating new systems, put the base constructor parameters in front of others,
              so that the Scene class can automatically fill them in.
    */
    class ComponentSystem {
    public:
        ComponentSystem(Scene* p_scene, KeyInterface* p_input) : m_scene(p_scene), m_input(p_input) {}
        virtual ~ComponentSystem() {}

        virtual void Init() {}
        virtual void Destroy() {}
        virtual void Update(float dt) {}
        virtual void Render() {}
    protected:
        // Scene on which this system acts.
        Scene* m_scene{ nullptr };
        // To provide input access for system.
        KeyInterface* m_input{ nullptr };
    };

    // System which handles rendering.
    class RenderSystem : public ComponentSystem {
    public:
        RenderSystem(Scene* p_scene, KeyInterface* p_input) : ComponentSystem(p_scene, p_input) {}

        void Render() override;
    };

    // System which handles native scripts.
    class NativeScriptSystem : public ComponentSystem {
    public:
        NativeScriptSystem(Scene* p_scene, KeyInterface* p_input) : ComponentSystem(p_scene, p_input) {}

        void Init() override;
        void Destroy() override;
        void Update(float dt) override;
    };

    class LuaScript;
    /// Manage all LuaScriptComponents and initialization/destruction of LuaScripts.
    class LuaScriptSystem : public ComponentSystem {
    public:
        LuaScriptSystem(Scene* p_scene, KeyInterface* p_input) : ComponentSystem(p_scene, p_input) {}

        /// Initializes all scripts currently bound to all LuaScriptComponents.
        void Init() override;
        /// Destroy all scripts currencly attached to the LuaScriptComponents.
        void Destroy() override;
        /// Update all LuaScripts that are attached to LuaScriptComponents.
        void Update(float dt) override;
        /// Initialize script for usage.
        /// @param ent Entity ID of the entity with LuaScriptComponent.
        /// @param name Name of the script specified when calling LuaScriptComponent::Attach().
        void InitScript(entt::entity ent, std::string name);
        /// Initialize script for usage.
        /// @param ent Entity ID of the entity with LuaScriptComponent.
        /// @param script Reference to the LuaScript, which is attached to the LuaScriptComponent.
        void InitScript(entt::entity ent, Ref<LuaScript> script);
        /// Destroy script. Call before detaching it from LuaScriptComponent.
        /// @param ent Entity ID of the entity with LuaScriptComponent.
        /// @param name Name of the script specified when calling LuaScriptComponent::Attach().
        void DestroyScript(entt::entity ent, std::string name);
        /// Destroy script. Call before detaching it from LuaScriptComponent.
        /// @param ent Entity ID of the entity with LuaScriptComponent.
        /// @param script Reference to the LuaScript, which is attached to the LuaScriptComponent.
        void DestroyScript(entt::entity ent, Ref<LuaScript> script);
    };

    // Handles updating of all rigid bodies.
    class PhysicsSystem : public ComponentSystem {
        const b2Vec2 GRAVITY{ 0.0f, -9.81f };

        class ContactListener : public b2ContactListener {
            PhysicsSystem* m_sys{ nullptr };
        public:
            ContactListener(PhysicsSystem* sys) : m_sys(sys) {}

            void BeginContact(b2Contact* contact) override;
            void EndContact(b2Contact* contact) override;
            void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
            void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
        };
    public:
        // Render outlines of the shapes.
        // TODO: For now, only supports rectangle shapes. Implement more shapes (eg. circles and polygons)
        bool m_DebugRender{ false };
        // Time of a single physics frame.
        // TODO: implement this. System should be running in separate thread and have a stable refresh rate.
        float m_RefreshRate{ 1.0f / 60.0f };
        // These are the recommended number of iterations from box2d docs.
        // If you need more/less precision you can try tweaking these values.
        int32_t m_VelocityIterations{ 6 };
        int32_t m_PositionIterations{ 2 };

        PhysicsSystem(Scene* scene, KeyInterface* input);

        void Init() override;
        void Destroy() override;
        void Update(float dt) override;
        void Render() override;

        // Set positions of rigidbody to transform component, create body in physics world and create its fixture.
        // Use this when you add RigidBodyComponent **after** the Scene::Init() method was called.
        void InitPhysicsBody(entt::entity ent);
        // Cleanup any data allocated on heap by this object.
        void CleanupPhysicsBody(entt::entity ent);

    private:
        Ref<b2World> m_physWorld{ nullptr };
        ContactListener m_contactListener;
    };
} // namespace Ren
