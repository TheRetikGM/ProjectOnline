#include "Ren/ECS/ComponentSystems.h"
#include "Ren/ECS/Scene.h"
#include "Ren/ECS/NativeScript.h"
#include "Ren/Renderer/Renderer.h"
#include "Ren/Physics/Physics.h"

using namespace Ren;

#pragma region --> Render system

void RenderSystem::Render()
{
    // Render all sprites.
    auto view = m_scene->SceneView<TransformComponent, SpriteComponent>();
    for (auto&& ent : view)
    {
        auto [trans, sprite] = view.get(ent);
        Renderer::SetRenderLayer(trans.layer);
        Renderer::RenderQuad({ trans.position - trans.scale * 0.5f, trans.scale }, trans.rotation, sprite.color, sprite.GetTexture());
    }
}

#pragma endregion
#pragma region --> NativeScript system

// Helper function to iterate through all entities, that have bound script.
template<typename Func>
constexpr void for_each_script(Scene* scene, Func func)
{
    auto view = scene->m_Registry->view<NativeScriptComponent>();
    for (auto&& ent : view)
    {
        auto [script] = view.get(ent);
        if (script.script_instance)
            func(ent, script.script_instance);
    }
}

inline void unbind_script(entt::registry& reg, entt::entity ent) { reg.get<NativeScriptComponent>(ent).Unbind(); }

void NativeScriptSystem::Init()
{
    m_scene->m_Registry->on_destroy<NativeScriptComponent>().connect<&unbind_script>();

    for_each_script(m_scene, [this](entt::entity ent, NativeScript* script) {
        script->m_entity = Entity{ ent, this->m_scene };
        script->m_input = this->m_input;
        script->OnInit();
    });
}
void NativeScriptSystem::Destroy()
{
    for_each_script(m_scene, [this](entt::entity ent, NativeScript* script) {
        script->OnDestroy();
    });
}
void NativeScriptSystem::Update(float dt)
{
    for_each_script(m_scene, [&dt](entt::entity ent, NativeScript* script) {
        script->OnUpdate(dt);
    });
}
#pragma endregion
#pragma region --> Physics system
// Physics system.
void PhysicsSystem::Init()
{
    if (!m_scene->m_PhysWorld)
        return;

    auto view = m_scene->SceneView<RigidBodyComponent, TransformComponent, TagComponent>();
    for (auto&& ent : view)
        m_scene->InitPhysicsBody({ ent, m_scene });
}
void PhysicsSystem::Destroy()
{
    if (!m_scene->m_PhysWorld)
        return;

    // Destroy all bodies in world when the world is deleted.
    for (b2Body* body = m_scene->m_PhysWorld->GetBodyList(); body;)
    {
        b2Body* next = body->GetNext();
        m_scene->m_PhysWorld->DestroyBody(body);
        body = next;
    }
    
    // Set pointers of the components to null, so that they don't point to free'd memory.
    auto view = m_scene->SceneView<RigidBodyComponent>();
    for (auto&& ent : view)
    {
        auto [r] = view.get(ent);
        r.p_body = nullptr;
        delete r.p_shape;
    }
    m_scene->m_PhysWorld.reset();
}
void PhysicsSystem::Update(float dt)
{
    if (!m_scene->m_PhysWorld)
        return;

    // Update physics.
    m_scene->m_PhysWorld->Step(dt, m_VelocityIterations, m_PositionIterations);
    
    // Sync TransformComponent position with RigidBodyComponent position.
    // TODO: Call collision callbacks.
    auto view = m_scene->SceneView<TransformComponent, RigidBodyComponent>();
    for (auto&& ent : view)
    {
        auto [trans, rigid] = view.get(ent);
        trans.position = Utils::to_vec2(rigid.p_body->GetPosition());
    }
}
void PhysicsSystem::Render()
{
    if (!m_DebugRender)
        return;

    Ren::Renderer::SetRenderLayer(1000);
    const auto draw_body = [&](b2Body* body){
        b2PolygonShape* shape = (b2PolygonShape*)body->GetFixtureList()->GetShape();
        REN_ASSERT(shape->m_count == 4, "Only rectangles are supported for now.");

        glm::vec2 pos = Utils::to_vec2(body->GetPosition());
        Ren::Rect rect{ pos + Utils::to_vec2(shape->m_vertices[0]), Utils::to_vec2(shape->m_vertices[2]) - Utils::to_vec2(shape->m_vertices[0]) };
        Ren::Renderer::DrawRect(rect, glm::degrees(body->GetAngle()), Ren::Colors4::White);
    };
    for (b2Body* body = m_scene->m_PhysWorld->GetBodyList(); body; body = body->GetNext())
        draw_body(body);
}

#pragma endregion