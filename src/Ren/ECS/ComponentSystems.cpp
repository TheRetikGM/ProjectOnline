#include "Ren/ECS/ComponentSystems.h"
#include "Ren/ECS/Scene.h"
#include "Ren/Scripting/NativeScript.h"
#include "Ren/Renderer/Renderer.h"
#include "Ren/Physics/Physics.h"
#include "Ren/Scripting/LuaScript.h"
#include "Ren/ECS/Components.h"

#include <ren_utils/logging.hpp>

using namespace Ren;

#pragma region --> Render system

void RenderSystem::Render() {
    // Render all sprites.
    auto view = m_scene->SceneView<TransformComponent, SpriteComponent>();
    for (auto&& ent : view)
    {
        auto [trans, sprite] = view.get(ent);
        Renderer::SetRenderLayer(trans.layer);

        glm::vec2 size = sprite.GetSize();

        Renderer::RenderQuad({ trans.position - size * 0.5f, size }, trans.rotation, sprite.m_Color, sprite.GetTexture());
    }
}

#pragma endregion
#pragma region --> NativeScript system

// Helper function to iterate through all entities, that have bound script.
template<typename Func>
void for_each_native_script(Scene* scene, Func func) {
    auto view = scene->m_Registry->view<NativeScriptComponent>();
    for (auto&& ent : view) {
        auto [script] = view.get(ent);
        if (script.script_instance)
            func(ent, script.script_instance);
    }
}

inline void unbind_script(entt::registry& reg, entt::entity ent) { reg.get<NativeScriptComponent>(ent).Unbind(); }

void NativeScriptSystem::Init() {
    m_scene->m_Registry->on_destroy<NativeScriptComponent>().connect<&unbind_script>();

    for_each_native_script(m_scene, [this](entt::entity ent, NativeScript* script) {
        script->m_entity = Entity{ ent, this->m_scene };
        script->m_input = this->m_input;
        script->OnInit();
    });
}
void NativeScriptSystem::Destroy() {
    for_each_native_script(m_scene, [](entt::entity ent, NativeScript* script) {
        script->OnDestroy();
    });
}
void NativeScriptSystem::Update(float dt) {
    for_each_native_script(m_scene, [&dt](entt::entity ent, NativeScript* script) {
        script->OnUpdate(dt);
    });
}
#pragma endregion

#pragma reqion --> LuaScript system
void LuaScriptSystem::InitScript(entt::entity ent, std::string name) {
    // Set data the script needs and initialize it.
    Entity e = { ent, m_scene };
    auto& comp = e.Get<LuaScriptComponent>();
    auto& script = comp.scripts.at(name);
    script->m_entity = e;
    script->m_input = m_input;
    script->init();
    script->OnInit();
}
void LuaScriptSystem::InitScript(entt::entity ent, Ref<LuaScript> script) {
    script->m_entity = { ent, m_scene };
    script->m_input = m_input;
    script->init();
    script->OnInit();
}

void LuaScriptSystem::DestroyScript(entt::entity ent, std::string name) {
    Entity e = { ent, m_scene };
    auto& comp = e.Get<LuaScriptComponent>();
    auto& script = comp.scripts.at(name);
    script->OnDestroy();
    script->destroy();
    script->m_entity = Entity{};
    script->m_input = nullptr;
}
void LuaScriptSystem::DestroyScript(entt::entity ent, Ref<LuaScript> script) {
    script->OnDestroy();
    script->destroy();
    script->m_entity = Entity{};
    script->m_input = nullptr;
}

template<typename Func>
void for_each_lua_script(Scene* scene, Func pred) {
    auto view = scene->SceneView<LuaScriptComponent>();
    for (auto&& ent : view) {
        auto [lsc] = view.get(ent);
        for (auto& [name, script] : lsc.scripts)
            pred(ent, script);
    }
}
void LuaScriptSystem::Init() {
    for_each_lua_script(m_scene, [this](entt::entity ent, Ref<LuaScript> script){
        InitScript(ent, script);
    });
}
void LuaScriptSystem::Destroy() {
    for_each_lua_script(m_scene, [this](entt::entity ent, Ref<LuaScript> script){
        DestroyScript(ent, script);
    });
}
void LuaScriptSystem::Update(float dt) {
    for_each_lua_script(m_scene, [dt](entt::entity ent, Ref<LuaScript> script){
        script->OnUpdate(dt);
    });
}
#pragma endregion

#pragma region --> Physics system
PhysicsSystem::PhysicsSystem(Scene* scene, KeyInterface* input)
    : ComponentSystem(scene, input)
    , m_physWorld(CreateRef<b2World>(GRAVITY))
    , m_contactListener(this)
{
    m_physWorld->SetContactListener(&m_contactListener);
}
// Physics system.
void PhysicsSystem::Init() {
    auto view = m_scene->SceneView<RigidBodyComponent>();
    for (auto&& ent : view)
        InitPhysicsBody(ent);
}
void PhysicsSystem::Destroy() {

    // Destroy all bodies in world when the world is deleted.
    for (b2Body* body = m_physWorld->GetBodyList(); body;) {
        b2Body* next = body->GetNext();
        Entity* ent = reinterpret_cast<Entity*>(body->GetUserData().pointer);
        CleanupPhysicsBody(ent->id);
        body = next;
    }

    // Set pointers of the components to null, so that they don't point to free'd memory.
    auto view = m_scene->SceneView<RigidBodyComponent>();
    for (auto&& ent : view) {
        auto [r] = view.get(ent);
        r.p_body = nullptr;
    }

    m_physWorld.reset();
}
void PhysicsSystem::Update(float dt) {
    if (!m_physWorld)
        return;

    // Update physics.
    m_physWorld->Step(dt, m_VelocityIterations, m_PositionIterations);

    // Sync TransformComponent position with RigidBodyComponent position.
    // TODO: Call collision callbacks.
    auto view = m_scene->SceneView<TransformComponent, RigidBodyComponent>();
    for (auto&& ent : view) {
        auto [trans, rig] = view.get(ent);
        if (trans.dirty) {
            rig.p_body->SetTransform(Utils::to_b2Vec2(trans.position), rig.p_body->GetAngle());
            trans.dirty = false;
        }
        else
            trans.position = Utils::to_vec2(rig.p_body->GetPosition());
        trans.rotation = rig.p_body->GetAngle() * (180.0f / 3.141592f);
    }
}
void PhysicsSystem::Render() {
    if (!m_DebugRender)
        return;

    Ren::Renderer::SetRenderLayer(1000);
    const auto draw_body = [&](b2Body* body){
        glm::vec2 pos = Utils::to_vec2(body->GetPosition());

        REN_ASSERT(body->GetFixtureList(), "Body does not have a fixture!");

        // Pre-calculate sin and cos for polygon rotation purposes.
        float sin_a = std::sin(body->GetAngle());
        float cos_a = std::cos(body->GetAngle());
        const auto rot_point = [sin_a, cos_a](const glm::vec2& p) -> glm::vec2 {
            return { p.x * cos_a - p.y * sin_a,
                     p.x * sin_a + p.y * cos_a };
        };

        for (auto fix = body->GetFixtureList(); fix; fix = fix->GetNext()) {
            switch (fix->GetShape()->GetType()) {
            case b2Shape::Type::e_polygon: {
                b2PolygonShape* shape = (b2PolygonShape*)fix->GetShape();

                // If polygon is rectangle and is not offseted, we can render it as a retangle.
                if (shape->m_count == 4 && Utils::to_vec2(shape->m_centroid) == glm::vec2(0.0f)) {
                    Ren::Rect rect{ pos + Utils::to_vec2(shape->m_vertices[0]), Utils::to_vec2(shape->m_vertices[2]) - Utils::to_vec2(shape->m_vertices[0]) };
                    Ren::Renderer::DrawRect(rect, glm::degrees(body->GetAngle()), Ren::Colors4::White);
                }
                // Otherwise we render lines between every vertex.
                else {
                    for (int i = 0; i < shape->m_count; i++) {
                        glm::vec2 a = Utils::to_vec2(shape->m_vertices[i]);
                        glm::vec2 b = Utils::to_vec2(shape->m_vertices[(i + 1) % shape->m_count]);
                        a = rot_point(a) + pos;
                        b = rot_point(b) + pos;

                        Ren::Renderer::DrawLine(a, b, Ren::Colors4::White);
                    }
                }
                } break;
            case b2Shape::Type::e_circle: {
                b2CircleShape* shape = (b2CircleShape*)fix->GetShape();
                Ren::Renderer::DrawCircle(pos + Utils::to_vec2(shape->m_p), shape->m_radius, Ren::Colors4::White);
                } break;
            default:
                LOG_W("Only polygon and circle shapes are supported for debug printing.");
                break;
            }
        }

    };
    for (b2Body* body = m_physWorld->GetBodyList(); body; body = body->GetNext())
        draw_body(body);
}
void PhysicsSystem::InitPhysicsBody(entt::entity raw_ent) {
    Entity ent = { raw_ent, m_scene };
    REN_ASSERT((ent.HasAll<RigidBodyComponent, TransformComponent>()), "Body must have RigidBodyComponent and TransformComponent to be initialized in physics world.");

    auto [rig, trans] = ent.GetM<RigidBodyComponent, TransformComponent>();

    // Body was already initialized.
    if (rig.p_body)
        return;

    // By default, position body at its transform.
    rig.body_def.position = Utils::to_b2Vec2(trans.position);

    // Each body has a custom data pointer to Entity structure allocated on heap (to be freed in Scene::CleanupPhysicsBody)
    Entity* p_entity = new Entity();
    *p_entity = ent;
    rig.body_def.userData.pointer = reinterpret_cast<uintptr_t>(p_entity);

    // Create body in physics world.
    rig.p_body = m_physWorld->CreateBody(&rig.body_def);

    // Create all fixtures of a body.
    for (auto&& [p_shape, fixture_def] : rig.fixtures) {
        REN_ASSERT(p_shape, "Body must have a shape. Entity tag = " + ent.GetTags().front());

        fixture_def.shape = p_shape.get();
        rig.p_body->CreateFixture(&fixture_def);
    }
}
void PhysicsSystem::CleanupPhysicsBody(entt::entity raw_ent) {
    Entity ent = { raw_ent, m_scene };
    REN_ASSERT((ent.HasAll<RigidBodyComponent>()), "Body must have RigidBodyComponent to be cleanedup");
    auto& rig = ent.Get<RigidBodyComponent>();
    REN_ASSERT(rig.p_body, "Rigid body was not initialized or was deleted in runtime.");

    // Delete from physics world (this internaly calls ContactListener::ContanctEnd(), so we have to make sure the custom data, which our
    // implementation of this listener uses, is not deleted by then).
    m_physWorld->DestroyBody(rig.p_body);

    // Delete custom data pointer to the parent entity.
    if (!rig.body_def.userData.pointer)
        return;
    Entity* p_ent = reinterpret_cast<Entity*>(rig.body_def.userData.pointer);
    delete p_ent;

    rig.body_def.userData.pointer = 0;
    rig.p_body = nullptr;
}

// Call function with arguments on each native script instance in scene.
template<typename Fun, typename... Args>
inline void call_on_nativescript(Scene* scene, Fun fun, b2Contact* contact, Args... args) {
    auto fixA = contact->GetFixtureA();
    auto fixB = contact->GetFixtureB();

    Entity* p_entA = reinterpret_cast<Entity*>(fixA->GetBody()->GetUserData().pointer);
    Entity* p_entB = reinterpret_cast<Entity*>(fixB->GetBody()->GetUserData().pointer);

    const auto call_fun = [&](NativeScript* instance, Entity e) { if (instance) (instance->*fun)(e, contact, args...); };

    if (p_entA->HasAll<NativeScriptComponent>())
        call_fun(p_entA->Get<NativeScriptComponent>().script_instance, *p_entB);
    if (p_entB->HasAll<NativeScriptComponent>())
        call_fun(p_entB->Get<NativeScriptComponent>().script_instance, *p_entA);
}

void PhysicsSystem::ContactListener::BeginContact(b2Contact* contact) {
    call_on_nativescript(m_sys->m_scene, &NativeScript::OnContactBegin, contact);
}
void PhysicsSystem::ContactListener::EndContact(b2Contact* contact) {
    call_on_nativescript(m_sys->m_scene, &NativeScript::OnContactEnd, contact);
}
void PhysicsSystem::ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    call_on_nativescript(m_sys->m_scene, &NativeScript::OnContactPreSolve, contact, oldManifold);
}
void PhysicsSystem::ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
    call_on_nativescript(m_sys->m_scene, &NativeScript::OnContactPostSolve, contact, impulse);
}
#pragma endregion
