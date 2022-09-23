#pragma once
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <regex>

#include "Ren/ECS/Scene.h"
#include "Ren/ECS/Components.h"
#include "Ren/Core/Core.h"
#include "Ren/Utils/Logger.hpp"

namespace YAML {
    template<>
    struct convert<glm::vec3> {
        static Node encode(const glm::vec3& rhs) {
            Node n;
            n.SetStyle(YAML::EmitterStyle::Flow);
            n[0] = rhs.x; n[1] = rhs.y; n[2] = rhs.z;
            return n;
        }
        static bool decode(const Node& node, glm::vec3& rhs) {
            if (!node.IsSequence() || node.size() != 3)
                return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::vec2> {
        static Node encode(const glm::vec2& rhs) {
            Node n;
            n.SetStyle(YAML::EmitterStyle::Flow);
            n[0] = rhs.x; n[1] = rhs.y;
            return n;
        }
        static bool decode(const Node& node, glm::vec2& rhs) {
            if (!node.IsSequence() || node.size() != 2)
                return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::ivec3> {
        static Node encode(const glm::ivec3& rhs) {
            Node n;
            n.SetStyle(YAML::EmitterStyle::Flow);
            n[0] = rhs.x; n[1] = rhs.y; n[2] = rhs.z;
            return n;
        }
        static bool decode(const Node& node, glm::ivec3& rhs) {
            if (!node.IsSequence() || node.size() != 3)
                return false;
            rhs.x = node[0].as<int>();
            rhs.y = node[1].as<int>();
            rhs.z = node[2].as<int>();
            return true;
        }
    };
    template<>
    struct convert<glm::ivec2> {
        static Node encode(const glm::ivec2& rhs) {
            Node n;
            n.SetStyle(YAML::EmitterStyle::Flow);
            n[0] = rhs.x; n[1] = rhs.y;
            return n;
        }
        static bool decode(const Node& node, glm::ivec2& rhs) {
            if (!node.IsSequence() || node.size() != 2)
                return false;
            rhs.x = node[0].as<int>();
            rhs.y = node[1].as<int>();
            return true;
        }
    };
    template<>
    struct convert<b2Vec2> {
        static Node encode(const b2Vec2& s) {
            Node n;
            n = *(const glm::vec2*)(&s);
            return n;
        }
        static bool decode(const Node& node, b2Vec2& s) {
            glm::vec2 vec = node.as<glm::vec2>();
            s = *(b2Vec2*)(&vec);
            return true;
        }
    };
    template<>
    struct convert<Ren::TransformComponent> {
        static Node encode(const Ren::TransformComponent& t) {
            Node n;
            n["position"] = t.position;
            n["scale"] = t.scale;
            n["layer"] = t.layer;
            return n;
        }
        static bool decode(const Node& node, Ren::TransformComponent& t) {
            t.position = node["position"].as<glm::vec2>();
            t.scale = node["scale"].as<glm::vec2>();
            t.layer = node["layer"].as<int>();
            return true;
        }
    };
    template<>
    struct convert<Ren::SpriteComponent> {
        static Node encode(const Ren::SpriteComponent& s) {
            Node n;
            n["img_path"] = std::regex_replace(s.img_path.string(), std::regex(ASSETS_DIR), "");
            n["color"] = s.m_Color;
            n["ppu"] = s.m_PixelsPerUnit;
            return n;
        }
        static bool decode(const Node& node, Ren::SpriteComponent& s) {
            s.img_path = ASSETS_DIR + node["img_path"].as<std::string>();
            s.m_Color = node["color"].as<glm::vec3>();
            s.m_PixelsPerUnit = node["ppu"].as<glm::ivec2>();
            return true;
        }
    };
    template<>
    struct convert<b2BodyDef> {
        static Node encode(const b2BodyDef& s) {
            Node n;
            switch (s.type) {
                case b2_dynamicBody:   n["type"] = "dynamic"; break;
                case b2_staticBody:    n["type"] = "static"; break;
                case b2_kinematicBody: n["type"] = "kinematic"; break;
            }
            n["position"] = s.position;
            n["angle"] = s.angle;
            n["linear_velocity"] = s.linearVelocity;
            n["angular_velocity"] = s.angularVelocity;
            n["linear_damping"] = s.linearDamping;
            n["angular_damping"] = s.angularDamping;
            n["allow_sleep"] = s.allowSleep;
            n["awake"] = s.awake;
            n["fixed_rotation"] = s.fixedRotation;
            n["bullet"] = s.bullet;
            n["enabled"] = s.enabled;
            n["gravity_scale"] = s.gravityScale;
            return n;
        }
        static bool decode(const Node& n, b2BodyDef& s) {
            std::string type = n["type"].as<std::string>();
            if (type == "dynamic")        s.type = b2_dynamicBody;
            else if (type == "static")    s.type = b2_staticBody;
            else if (type == "kinematic") s.type = b2_kinematicBody;
            
            s.position        = n["position"].as<b2Vec2>();
            s.angle           = n["angle"].as<float>();
            s.linearVelocity  = n["linear_velocity"].as<b2Vec2>();
            s.angularVelocity = n["angular_velocity"].as<float>();
            s.linearDamping   = n["linear_damping"].as<float>();
            s.angularDamping  = n["angular_damping"].as<float>();
            s.allowSleep      = n["allow_sleep"].as<bool>();
            s.awake           = n["awake"].as<bool>();
            s.fixedRotation   = n["fixed_rotation"].as<bool>();
            s.bullet          = n["bullet"].as<bool>();
            s.enabled         = n["enabled"].as<bool>();
            s.gravityScale    = n["gravity_scale"].as<float>();
            return true;
        }
    };
    template<>
    struct convert<b2FixtureDef> {
        static Node encode(const b2FixtureDef& s) {
            Node n;
            if (s.shape->m_type == b2Shape::Type::e_polygon) {
                n["shape_type"] = "polygon";
                auto poly = (const b2PolygonShape*)s.shape;
                n["shape"]["centroid"] = poly->m_centroid;
                n["shape"]["vertices"].SetStyle(YAML::EmitterStyle::Flow);
                n["shape"]["normals"].SetStyle(YAML::EmitterStyle::Flow);
                for (int i = 0; i < poly->m_count; i++) {
                    n["shape"]["vertices"][i] = poly->m_vertices[i];
                    n["shape"]["normals"][i] = poly->m_normals[i];
                }
                n["shape"]["count"] = poly->m_count;
            }
            else if (s.shape->m_type == b2Shape::Type::e_circle) {
                n["shape_type"] = "circle";
                auto cir = (const b2CircleShape*)s.shape;
                n["shape"]["radius"] = cir->m_radius;
                n["shape"]["position"] = cir->m_p;
            }
            else LOG_E("Serialization of shape with b2Shape::Type == " + std::to_string(s.shape->m_type) + " is not supported yet.");

            n["friction"] = s.friction;
            n["restitution"] = s.restitution;
            n["restitution_threshold"] = s.restitutionThreshold;
            n["density"] = s.density;
            n["is_sensor"] = s.isSensor;
            n["filter"]["category_bits"] = s.filter.categoryBits;
            n["filter"]["mask_bits"] = s.filter.maskBits;
            n["filter"]["group_index"] = s.filter.groupIndex;

            return n;
        }
        static bool decode(const Node& n, b2FixtureDef& s) {
            s.friction = n["friction"].as<float>();
            s.restitution = n["restitution"].as<float>();
            s.restitutionThreshold = n["restitution_threshold"].as<float>();
            s.density = n["density"].as<float>();
            s.isSensor = n["is_sensor"].as<bool>();
            s.filter.categoryBits = (uint16)n["filter"]["category_bits"].as<int>();
            s.filter.maskBits = (uint16)n["filter"]["mask_bits"].as<int>();
            s.filter.groupIndex = (int16)n["filter"]["group_index"].as<int>();
            return true;
        }
    };
    template<>
    struct convert<Ren::RigidBodyComponent> {
        static Node encode(const Ren::RigidBodyComponent& s) {
            Node n;
            REN_ASSERT(s.p_body, "PhysicsBodyComponent is not initialized. See Scene::InitPhysicsBody().");

            n["body_def"] = s.body_def;
            for (auto&& [p_shape, fix_def] : s.fixtures)
                n["fixtures"].push_back(fix_def);

            return n;
        }
        static bool decode(const Node& n, Ren::RigidBodyComponent& s) {
            s.body_def = n["body_def"].as<b2BodyDef>();
                        
            for (auto&& n_fix : n["fixtures"])
            {
                b2FixtureDef fix_def = n_fix.as<b2FixtureDef>();
                Ref<b2Shape> shape = nullptr;

                if (n_fix["shape_type"].as<std::string>() == "polygon") {
                    auto poly_shape = CreateRef<b2PolygonShape>();
                    YAML::Node ys = n_fix["shape"];
                    poly_shape->m_count = ys["count"].as<int>();
                    for (int i = 0; i < poly_shape->m_count; i++) {
                        poly_shape->m_vertices[i] = ys["vertices"][i].as<b2Vec2>();
                        poly_shape->m_normals[i] = ys["normals"][i].as<b2Vec2>();
                    }
                    poly_shape->m_centroid = ys["centroid"].as<b2Vec2>();
                    shape = poly_shape;
                }
                else if (n_fix["shape_type"].as<std::string>() == "circle") {
                    auto circle_shape = CreateRef<b2CircleShape>();
                    YAML::Node ys = n_fix["shape"];
                    circle_shape->m_radius = ys["radius"].as<float>();
                    circle_shape->m_p = ys["position"].as<b2Vec2>();
                    shape = circle_shape;
                }

                REN_ASSERT(shape, "Fixture has an unknown shape type. Type = " + n_fix["shape_type"].as<std::string>());
                s.fixtures.push_back({ shape, fix_def });
            }
            
            return true;
        }
    };
}
// template<>
// struct convert<Ren::RigidBodyComponent> {
//     static Node encode(const Ren::RigidBodyComponent& s) {
//         Node n;
//         return n;
//     }
//     static bool decode(const Node& n, Ren::RigidBodyComponent& s) {
//         return true;
//     }
// };