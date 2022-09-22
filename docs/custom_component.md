# Custom component and its integration with Ren
If you want to create a custom component, this is the tutorial for you.

## 1 Create a component structure
- It can be created anywhere in the project, but to keep things organized please put it into [Components.h](../src/include/Ren/ECS/Components.h).
- Example code:
    ```C++
    // Represents entity outline.
    struct OutlineComponent
    {
        glm::ivec4 color{ 255, 255, 255, 255 };     // Default white color.
        float      rotation{ 0.0f };    // Our outline can be rotated.
    };
    ```
- You can then add the component to entity
    ```C++
    // You can now add the component to entity
    Entity e = scene->CreateEntity();
    e.Add<OutlineComponent>();
    ```
## 2 Create a system
- Component system is a class, which handles all entities with your component (for example RenderSystem can render a SpriteComponents).
- Your system class must inherit from **Ren::ComponentSystem**.
- First two argument in constructor must be `Ren::Scene* p_scene` and `Ren::KeyInterface* p_input` (see example).
- Example code:
    ```C++
    class OutlineSystem : public Ren::ComponentSystem
    {
    public:
        // Valid
        OutlineSystem(Ren::Scene* p_scene, Ren::KeyInterface* p_input)
            : Ren::ComponentSystem(p_scene, p_input), m_renderer(renderer) {}
        // Valid
        OutlineSystem(Ren::Scene* p_scene, Ren::KeyInterface* p_input, float custom_val) 
            : Ren::ComponentSystem(p_scene, p_input), m_renderer(renderer), m_customVal(custom_val) {}
        // Invalid!! Missing Scene and KeyInterface params
        // OutlineSystem(float custom_val) : m_customVal(custom_val) {}

        // Render all outline components.
        void Render() override
        {
            // Scene view is an object, which helps us to iterate over all entities
            // that have given components.
            auto view = m_scene->SceneView<OutlineComponent, Ren::TransformComponent>();
            for (auto&& ent : view)
            {
                // Get components from entity.
                auto [outline, trans] = view.get(ent);

                // Render outline in correct layer.
                Ren::Renderer::SetRenderLayer(trans.layer);
                Ren::Renderer::DrawRect({ trans.position - trans.scale * 0.5f, trans.scale }, outline.rotation, outline.color);
            }
        }
    private:
        glm::vec2 m_rectPos{ 100, 100 };
        float m_customVal{ 0.0f };
    };
    ```
## 3 Add system to scene
- For system to be automatically updated, you have to create it within a scene. 
- To create a system use `Scene::AddSystem(args...)` function (see example)
- Where to put it?
    - You can put it in your code directly
        ```C++
        Ren::Scene* scene = new Scene(renderer, input);
        float custom_val = 12.3f;
        scene->AddSystem<OutlineSystem>(custom_val);
        ```
    - Or integrate it into Ren engine file [Scene.cpp](../src/Ren/ECS/Scene.cpp) (it will be created automatically on scene creation)
        ```C++
        // Scene.cpp
        
        // ... //

        Scene::Scene(SDL_Renderer* renderer, KeyInterface* input)
            : m_input(input)
        {
            // ... //

            AddSystem<OutlineSystem>();
        }

        // ... //

        ```
## 4 Serialization/Deserialization
For serialization, we use the [YAML-cpp](https://github.com/jbeder/yaml-cpp) library.
### 4.1 Define how
- To make serialization work, you have to define how the component should be serialized/deserialized.
    - We treat component as custom type and thus we need to tell YAML-cpp how it should be serialized. 
    - According to YAML-cpp [documentation](https://github.com/jbeder/yaml-cpp/wiki/Tutorial#converting-tofrom-native-data-types) we can do something like this:
        ```C++
        namespace YAML
        {
            template<>
            struct convert<OutlineComponent> {
                static Node encode(const OutlineComponent& s) 
                {
                    Node n;
                    n["color"]    = s.color;
                    n["rotation"] = s.rotation;

                    return n;
                }

                // Return false, if due to some reason you cannot deserialize it (for example, some unexpected values etc.)
                static bool decode(const Node& n, OutlineComponent& s) 
                {
                    s.color    = n["color"].as<glm::ivec4>();
                    s.rotation = n["rotation"].as<float>();

                    return true;
                }
            };
        }
        ```
### 4.2 Integrate with Ren::SceneSerializer
- For Ren::SceneSerializer to find our own convert functions, we have to put them somewhere, so that thay are awailble at compile time.
    - That means, we cannot put them in their own `.cpp` file
- To integrate with Ren, put them in a file called [YAMLConversions.hpp](../src/include/Ren/ECS/Serialization/YAMLConversions.hpp)
- Next, you have to tell Ren::SceneSerialization object to try to parse your component type.
    - To do that, we have to *add a template argument* to the EntitySerializer typedef available in [SceneSerializer.hpp](../src/include/Ren/ECS/Serialization/SceneSerializer.hpp) (see example)
        ```C++
        typedef EntitySerializerWrapper<
            TransformComponent,
            SpriteComponent,
            RigidBodyComponent,
            OutlineComponent    // <-- Here
        > EntitySerializer;
        ```
### 4.3 Define component setup
- Component setup is basically a function which defines how a component should be added to an entity and how to configure it with de-serialized data.
- For most components this is **NOT** needed, however if you need to construct a component in some special way, this is how you would do it.
    - For example a special case is **SpriteComponent** whih needs to be added with a path to texture, for it to be loaded by Scene object. Or it must be loaded afterward manually.
- The definition should be added to [ComponentSetups.hpp](../src/include/Ren/ECS/Serialization/ComponentSetups.hpp), but you can paste it where you want. However it has to be available to SceneSerializer at compile time.
- Example code:
    ```C++
    namespace Ren
    {
        template<>
        void setup_component<OutlineComponent>(Entity e, const OutlineComponent& comp)
        {
            auto& r = e.Add<OutlineComponent>();
            r.color = comp.color;
            r.rotation = comp.rotation;
            // Your special code here...
        }
    }
    ```
# 5 Be happy
... and get to debugging because it will probably not work on a first try.
