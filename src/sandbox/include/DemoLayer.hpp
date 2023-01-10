#pragma once
#include <Ren/Ren.h>
#include <box2d/box2d.h>
#include <fstream>

#include "scripts/Movement.hpp"

using namespace entt::literals;

class DemoLayer : public Ren::Layer 
{
    Ref<Ren::TextRenderer> m_textRenderer = Ren::TextRenderer::Create();
    Ref<Ren::Scene> m_scene;
    Ren::Entity m_ent;
    Ren::CartesianCamera m_camera;
    Ren::Texture2D m_renderTexture;

public:
    DemoLayer(const std::string &name) : Ren::Layer(name) {}

    void OnInit() override 
    {
        // Set background color.
        m_GameCore->m_ClearColor = {100, 100, 100, 255};

        sceneFromScratch();
        // sceneFromFile();

        // Load font.
        m_textRenderer->Load(ASSETS_DIR "fonts/DejaVuSansCondensed.ttf", 32);

        // Setup camera with initial scale of 50 pixels per unit.
        m_camera.SetUnitScale(50);
        m_camera.SetViewportSize(m_GameCore->GetWindowSize());

        m_renderTexture.m_Size = m_GameCore->GetWindowSize();
        m_renderTexture.m_Format = SDL_PIXELFORMAT_RGBA32;
        m_renderTexture.m_Access = SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET;
        m_renderTexture.Generate();
    }
    void OnDestroy() override 
    {
        // Destroy native script component.
        m_scene->Destroy();
        m_scene.reset();
    }
    void OnEvent(Ren::Event &e) override 
    {
        if (e.event.type == SDL_WINDOWEVENT && e.event.window.event == SDL_WINDOWEVENT_RESIZED)
            m_camera.SetViewportSize(m_GameCore->GetWindowSize());

        if (!(m_GameCore->m_Run = !Ren::Utils::key_pressed(e.event, SDLK_ESCAPE)))
            e.handled = true;
        if (e.event.type == SDL_MOUSEWHEEL) {
            if (e.event.wheel.y > 0) // scroll up
                m_camera.SetUnitScale(m_camera.GetUnitScale() + glm::ivec2(10));
            else
                m_camera.SetUnitScale(m_camera.GetUnitScale() - glm::ivec2(10));
        }
        if (e.event.type == SDL_MOUSEBUTTONDOWN && e.event.button.button == SDL_BUTTON_LEFT) {
            glm::ivec2 mouse_pos(0);
            SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
            glm::vec2 pos = m_camera.ToUnits(glm::vec2(mouse_pos));

            Ren::Entity dynamic_body =
            m_scene->CreateEntity({{pos.x, pos.y}}, {"spawned_body"});
            auto &dynamic_body_r = dynamic_body.Add<Ren::RigidBodyComponent>();
            auto box_shape = CreateRef<b2PolygonShape>();
            box_shape->SetAsBox(Ren::Utils::RandomFloat(0.5f, 2.0f),
                                Ren::Utils::RandomFloat(0.5f, 2.0f));
            dynamic_body_r.body_def.type = b2_dynamicBody;
            b2FixtureDef fix_def;
            fix_def.density = Ren::Utils::RandomFloat(0.5f, 5.0f);
            fix_def.friction = Ren::Utils::RandomFloat_0_1();
            fix_def.restitution = Ren::Utils::RandomFloat_0_1();
            dynamic_body_r.fixtures.push_back({box_shape, fix_def});

            m_scene->GetSystem<Ren::PhysicsSystem>()->InitPhysicsBody(
            dynamic_body.id);
        }

        if (Ren::Utils::key_pressed(e.event, SDLK_SPACE))
            m_camera.SetUnitScale(100);
    }
    void OnUpdate(float dt) override 
    {
        // Rotate entities with 'rotate' tag.
        const float rotation_speed = 90.0f; // 90 degrees per second.
        auto entities = m_scene->GetEntitiesByTag("rotate");
        for (auto &&ent : *entities)
            ent.Get<Ren::TransformComponent>().rotation += rotation_speed * dt;

        if (KeyPressed(Ren::Key::SPACE))
            m_camera.m_CamPos = glm::vec2(0.0f);

        float move_speed = 8.0f;
        if (KeyHeld(Ren::Key::LEFT))
            m_camera.m_CamPos -= Ren::RightDir() * move_speed * dt;
        if (KeyHeld(Ren::Key::RIGHT))
            m_camera.m_CamPos += Ren::RightDir() * move_speed * dt;
        if (KeyHeld(Ren::Key::UP))
            m_camera.m_CamPos += Ren::UpDir() * move_speed * dt;
        if (KeyHeld(Ren::Key::DOWN))
            m_camera.m_CamPos -= Ren::UpDir() * move_speed * dt;

        glm::ivec2 mouse_rel_pos(0);
        bool mouse_pressed = SDL_GetRelativeMouseState(&mouse_rel_pos.x, &mouse_rel_pos.y) & SDL_BUTTON_RMASK;
        if (mouse_pressed)
            m_camera.m_CamPos -= glm::vec2(mouse_rel_pos.x, -mouse_rel_pos.y) / glm::vec2(m_camera.GetUnitScale());

        m_scene->Update(dt);
    }
    void OnRender(SDL_Renderer *renderer) override 
    {
        Ren::Renderer::BeginRender(&m_camera, &m_renderTexture);
        Ren::Renderer::Clear(m_GameCore->m_ClearColor);
        m_textRenderer->RenderText(
            "WSAD for movement\n"
            "Arrow keys or hold mouse right button for camera movement\n"
            "'i' to toggle imgui demo window\n"
            "ESC to exit\n"
            "Mouse left button for spawning new body\n"
            "Mouse wheel for zoom",
            {10.0f, 10.0f}, 1.0f, Ren::Colors3::White, 10);
        m_scene->Render();
        Ren::Renderer::Render();
        Ren::Renderer::EndRender();
    }

private:
    void sceneFromFile(std::filesystem::path path = ASSETS_DIR "scenes/demo.ren") 
    {
        m_scene = Ren::SceneSerializer::Deserialze(path, GetRenderer(), GetInput());
        m_scene->GetSystem<Ren::PhysicsSystem>()->m_DebugRender = true;
        auto [success, ent] = m_scene->GetEntityByTag("awesomeface");
        if (success)
            ent.Add<Ren::NativeScriptComponent>().Bind<MovementScript>();
        m_scene->Init();
    }

    void sceneFromScratch(bool serialize = false) 
    {
        m_scene = CreateRef<Ren::Scene>(GetRenderer(), GetInput());

        // Create awesomeface entity.
        m_ent = m_scene->CreateEntity({glm::vec2(1.0f), glm::vec2(2.0f)}, {"awesomeface"});
        m_ent.Add<Ren::SpriteComponent>(ASSETS_DIR "awesomeface.png").m_Color = Ren::Colors3::Magenta;
        m_ent.Add<Ren::NativeScriptComponent>().Bind<MovementScript>();
        m_ent.Get<Ren::TransformComponent>().layer = 2;
        auto &rig = m_ent.Add<Ren::RigidBodyComponent>();
        {
            auto size = m_ent.Get<Ren::SpriteComponent>().GetSize() * 0.5f;
            rig.body_def.type = b2_dynamicBody;
            rig.body_def.fixedRotation = false;

            auto circle_shape = CreateRef<b2CircleShape>();
            circle_shape->m_radius = size.x;
            b2FixtureDef fix;
            fix.density = 1.0f;
            fix.friction = 0.3f;
            fix.restitution = 0.67f;
            rig.fixtures.push_back({circle_shape, fix});

            auto box_shape = CreateRef<b2PolygonShape>();
            box_shape->SetAsBox(size.x, size.y, {0, 0.5f}, 0.0f);
            rig.fixtures.push_back({box_shape, fix});
        }

        // Create copy of awesomeface entity (the texture will get reused).
        auto ent_copy = m_scene->CreateEntity({glm::vec2(0.0f), glm::vec2(2.0f)}, {"rotate"});
        ent_copy.Add<Ren::SpriteComponent>(ASSETS_DIR "awesomeface.png");
        ent_copy.Get<Ren::TransformComponent>().layer = 1;

        /////////// Box2D ///////////
        {
            auto box_shape = CreateRef<b2PolygonShape>();

            Ren::Entity ground_body = m_scene->CreateEntity(
            Ren::TransformComponent({0.0f, -10.0f}), Ren::TagList{"ground"});
            auto &ground_body_r = ground_body.Add<Ren::RigidBodyComponent>();
            box_shape->SetAsBox(50.0f, 10.0f);
            ground_body_r.fixtures.push_back({box_shape, {}});

            Ren::Entity another_body =
            m_scene->CreateEntity({{-1.7f, 2.0f}}, {"another_body"});
            auto &another_body_r = another_body.Add<Ren::RigidBodyComponent>();
            box_shape = CreateRef<b2PolygonShape>();
            box_shape->SetAsBox(1.0f, 1.0f);
            another_body_r.fixtures.push_back({box_shape, {}});

            // Dynamic body
            Ren::Entity dynamic_body = m_scene->CreateEntity({{0.0f, 10.0f}}, {"dynamic_body"});
            auto &dynamic_body_r = dynamic_body.Add<Ren::RigidBodyComponent>();
            box_shape = CreateRef<b2PolygonShape>();
            box_shape->SetAsBox(1.5f, 1.f);
            dynamic_body_r.body_def.type = b2_dynamicBody;
            b2FixtureDef fix_def;
            fix_def.density = 1.0f;
            fix_def.friction = 0.3f;
            fix_def.restitution = 0.67;
            dynamic_body_r.fixtures.push_back({box_shape, fix_def});
        }
        m_scene->GetSystem<Ren::PhysicsSystem>()->m_DebugRender = true;
        // Call init on all scene subsystems.
        m_scene->Init();

        if (serialize)
            Ren::SceneSerializer::Serialize(m_scene, SOURCE_DIR "/build/test.yaml");
    }

    friend class ImGuiLayer;
};
