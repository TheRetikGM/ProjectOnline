#pragma once
#include <filesystem>
#include <regex>

#include "Ren/ECS/Scene.h"
#include "Ren/ECS/Components.h"
#include "Ren/Core/Core.h"
#include "Ren/ECS/Serialization/YAMLConversions.hpp"
#include "Ren/ECS/Serialization/ComponentSetups.h"

namespace Ren {
    namespace Utils {
        // See https://stackoverflow.com/questions/28547456/call-void-function-for-each-template-type-in-a-variadic-templated-function for more details.
        template<class...Fs>
        void do_in_order( Fs&&...fs )
        {
            int _[]={0, (void(std::forward<Fs>(fs)()), 0)...};
            (void)_;
        }
    };

    /// Serialize scene and save it into a file.
    /// NOTE: For now, does not serialize System settings.
    class SceneSerializer {
    public:
        SceneSerializer() {};

        static void Serialize(Ref<Scene> scene, std::filesystem::path path) {
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

        /// Deserialize scene.
        /// NOTE: Returned scene is NOT initialized.
        static Ref<Scene> Deserialze(std::filesystem::path path, SDL_Renderer* renderer, KeyInterface* input) {
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
    private:

        template<typename... TComp>
        struct EntitySerializerWrapper {
            inline static int last_comp_id = 0;

            // Get unique ID for type (only in this context).
            template<typename T>
            static int getID() {
                static int id = last_comp_id++;
                return id;
            }

            static YAML::Node Serialize(Entity e) {
                // We need to get IDs of the components in the same order
                // for each scene. As such we create them in this dummy array
                // first.
                int _[] = {-1, getID<TComp>()...}; (void)_;

                YAML::Node ent_info;

                // FIXME: For now hard-set the UUID
                ent_info["UUID"] = 123;

                // Serialize tags.
                auto& tags = e.GetTags();
                for (auto&& tag : tags) {
                    ent_info["Tags"].SetStyle(YAML::EmitterStyle::Flow);
                    ent_info["Tags"].push_back(tag);
                }

                // Serialize all components, which are specified in class template.
                Utils::do_in_order([&]{
                    if (e.HasAll<TComp>()) {
                        int index = ent_info["Components"].size();
                        ent_info["Components"][index] = e.Get<TComp>();
                        ent_info["Components"][index]["ID"] = getID<TComp>();
                    }
                }...);

                return ent_info;
            }
            static void Deserialize(const YAML::Node& node, Entity& e) {
                // See explanation above.
                int _[] = {-1, getID<TComp>()...}; (void)_;

                // TODO: UUID

                // Add all tags to entity.
                for (auto&& tag : node["Tags"])
                    e.AddTag(tag.as<std::string>());

                // Add components to entity.
                for (auto&& component : node["Components"]) {
                    Utils::do_in_order([&]{
                        if (component["ID"].as<int>() == getID<TComp>())
                        {
                            TComp comp = component.as<TComp>();
                            setup_component(e, comp);
                        }
                    }...);
                }
            }
        };

        // Specify what components should be serialized.
        // Note: New components should be added to the end of
        //  of the list, so that we keep backward compatibility
        //  with serialized scenes.
        typedef EntitySerializerWrapper<
            TransformComponent,
            SpriteComponent,
            RigidBodyComponent,
            LuaScriptComponent
        > EntitySerializer;
    };
}
