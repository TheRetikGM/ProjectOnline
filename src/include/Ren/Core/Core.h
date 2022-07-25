#pragma once
#include <memory>
#include <iostream>
#include <glm/glm.hpp>

#ifdef REN_DEBUG
    #define REN_ASSERT(condition, message) \
        do { \
            if (! (condition)) { \
                std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                        << ":" << __LINE__ << ": " << message << std::endl; \
                std::terminate(); \
            } \
        } while (false)
    #define REN_STATUS(message) std::cout << "[Ren] -- " << message << std::endl
#else
    // FIXME: Print statement, to avoid compiler deleting the condition. That would not be problem, if the code in condition wouldn't need to be executed.
    #define REN_ASSERT(condition, message) if (!(condition)) std::cout << "[Ren-error] -- " << message << std::endl
    #define REN_STATUS(message) do {} while(false)
#endif

template<typename T>
using Ref = std::shared_ptr<T>;

template<class T, class... Args>
inline Ref<T> CreateRef(Args... args) { return std::make_shared<T>(std::forward<Args>(args)...); }

namespace Ren
{
    template<typename T>
    struct Colors
    {
        const inline static T Black =   T(glm::ivec4(  0,   0,   0, 255));
        const inline static T Red =     T(glm::ivec4(255,   0,   0, 255));
        const inline static T Green =   T(glm::ivec4(  0, 255,   0, 255));
        const inline static T Blue =    T(glm::ivec4(  0,   0, 255, 255));
        const inline static T Yellow =  T(glm::ivec4(255, 255,   0, 255));
        const inline static T Magenta = T(glm::ivec4(255,   0, 255, 255));
        const inline static T Cyan =    T(glm::ivec4(  0, 255, 255, 255));
        const inline static T White =   T(glm::ivec4(255, 255, 255, 255));
    };
    using Colors4 = Colors<glm::ivec4>;
    using Colors3 = Colors<glm::ivec3>;
    using Color3 = glm::ivec3;
    using Color4 = glm::ivec4;

    // Rectangle with float precision.
    struct Rect 
    { 
        glm::vec2 pos, size;
        float& x{ pos.x };
        float& y{ pos.y };
        float& w{ size.x };
        float& h{ size.y };

        Rect(glm::vec2 pos, glm::vec2 size) : pos(pos), size(size) {}
        Rect(float x, float y, float w, float h) : pos(x, y), size(w, h) {}
    };
}