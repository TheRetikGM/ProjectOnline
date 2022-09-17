#include <tuple>
#include <vector>

#include "Ren/ECS/Scene.h"
#include "Ren/Utils/Logger.hpp"

namespace Ren
{
    Scene::Scene(SDL_Renderer* renderer, KeyInterface* input)
        : m_input(input)
    {
        REN_ASSERT(renderer != nullptr, "Invalid renderer.");
        m_Renderer = renderer;

        if (!m_Registry)
            m_Registry = CreateRef<entt::registry>();
        else
            m_Registry->clear();
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
    }
    Scene::~Scene()
    {
        m_sysManager.Clear();
        m_textureCache->clear();
        m_Registry->clear();
    }

    Entity Scene::CreateEntity(const TransformComponent& transform_comp, const TagComponent& tag_comp)
    {
        Entity ent{ m_Registry->create(), this };
        ent.Add<TransformComponent>() = transform_comp;
        ent.Add<TagComponent>() = tag_comp;
        return ent;
    };

    Ref<std::vector<Entity>> Scene::GetEntitiesByTag(const std::string& tag)
    {
        auto view = m_Registry->view<TagComponent>();
        auto matches = CreateRef<std::vector<Entity>>();
        for (auto&& ent : view)
            if (view.get<TagComponent>(ent).tag == tag)
                matches->push_back({ ent, this });
        return matches;
    }

    std::tuple<bool, Entity> Scene::GetEntityByTag(const std::string& tag)
    {
        auto view = m_Registry->view<TagComponent>();

        for (auto&& ent : view)
            if (view.get<TagComponent>(ent).tag == tag)
                return std::make_tuple(true, Entity{ ent, this });

        return std::make_tuple(false, Entity{});
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

    void Scene::InitPhysicsBody(Entity ent)
    {
        auto [rig, trans, tag] = ent.GetM<RigidBodyComponent, TransformComponent, TagComponent>();
        
        // Body was already initialized.
        if (rig.p_body)
            return;

        REN_ASSERT(rig.p_shape, "Body must have a shape. Body tag = " + tag.tag);

        // Create body and its fixture.
        rig.body_def.position = Utils::to_b2Vec2(trans.position);
        rig.p_body = m_PhysWorld->CreateBody(&rig.body_def);
        rig.fixture_def.shape = rig.p_shape;
        rig.p_body->CreateFixture(&rig.fixture_def);
    }

} // namespace Ren
