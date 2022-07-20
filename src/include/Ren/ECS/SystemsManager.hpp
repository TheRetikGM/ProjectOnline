#pragma once
#include <unordered_map>
#include <typeinfo>

#include "Ren/Core/Core.h"
#include "ComponentSystems.h"

namespace Ren
{
    class SystemsManager
    {
    public: 
        // Create new system of specified type.
        // NOTE: Type must inherit from ComponentSystem base class.
        template<typename T, typename... Args>
        void Add(Args... args)
        {
            int32_t sys_id = getID<T>();

            // Check if system was already created.
            REN_ASSERT(m_systems.count(sys_id) == 0, "System of type '" + std::string(typeid(T).name()) + "' already exists.");
            
            // Create new instance, down-cast it to base class type and store it in Ref.
            m_systems[sys_id] = Ref<ComponentSystem>(dynamic_cast<ComponentSystem*>(new T(std::forward<Args>(args)...)));
        }

        // Return specified system.
        template<typename T>
        inline T* Get()
        {
            int32_t sys_id = getID<T>();
            return m_systems.count(sys_id) ? dynamic_cast<T*>(m_systems[sys_id].get()) : nullptr;
        }

        // Remove specified system.
        template<typename T>
        inline void Remove()
        {
            int32_t sys_id = getID<T>();
            REN_ASSERT(m_systems.count(sys_id) == 1, "Trying to remove non-existent system of type '" + std::string(typeid(T).name()) + "'.");
            m_systems.erase(sys_id);
        }

        // Call Init on all systems.
        inline void Init() { for (auto&& [sys_id, sys] : m_systems) sys->Init(); }

        // Call Destroy on all systems.
        inline void Destroy() { for (auto&& [sys_id, sys] : m_systems) sys->Destroy(); }

        // Call Update on all systems.
        inline void Update(float dt) { for (auto&& [sys_id, sys] : m_systems) sys->Update(dt); }

        // Call Render on all systems.
        inline void Render() { for (auto&& [sys_id, sys] : m_systems) sys->Render(); }

        // Remove all systems.
        inline void Clear() { m_systems.clear(); }

    private:
        std::unordered_map<int32_t, Ref<ComponentSystem>> m_systems{};
        inline static int32_t ms_lastSystemID = 0;

        // Get ID of specified system type.
        template<typename T>
        inline static int32_t getID()
        {
            static int32_t id = ms_lastSystemID++;
            return id;
        }
    };
} // namespace Ren
