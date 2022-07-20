#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL.h>

#include "Ren/Core/Core.h"
#include "Ren/RenSDL/Context.hpp"

namespace Ren
{
    struct RenRect
    {
        float x, y, w, h;
    };

    class Camera
    {
    public:
        // Camera position in unit space.
        glm::vec2 m_CamPos{ .0f, .0f };

        virtual glm::mat4 GetPV() = 0;
        // Apply transformations and return SDL_Rect, which can be used for rendering.
        virtual SDL_Rect ConvertRect(const RenRect& rect, const glm::mat4& pv) = 0;
        virtual glm::vec2 UpDir() const = 0;
        virtual glm::vec2 RightDir() const = 0;

        // Set viewport size in pixels.
        inline void SetViewportSize(const glm::ivec2& size) { m_viewportSize = size; }
        inline void SetUnitScale(uint32_t pixels_per_unit) { m_pixelsPerUnit = glm::ivec2(pixels_per_unit); }
        inline void SetUnitScale(glm::ivec2 pixels_per_unit) { m_pixelsPerUnit = pixels_per_unit; }
        inline const glm::ivec2& GetUnitScale() { return m_pixelsPerUnit; }
        inline SDL_Rect ConvertRect(const RenRect& rect) { return ConvertRect(rect, GetPV()); };
        inline glm::vec2 GetSize() { return glm::vec2(m_viewportSize) / glm::vec2(m_pixelsPerUnit); }
    protected:
        // Size of viewport (window) in pixels.
        glm::ivec2 m_viewportSize{ 1, 1 };
        glm::ivec2 m_pixelsPerUnit{ 1, 1 };
    };

    class PixelCamera : public Camera
    {
        inline glm::vec2 UpDir() const { return { 0.0f, -1.0f }; }
        inline glm::vec2 RightDir() const { return { 1.0f, 0.0f }; }
        inline glm::mat4 GetPV() override
        {
            glm::mat4 view(1.0f);
            view = glm::translate(view, { -m_CamPos, 0.0f });
            view = glm::scale(view, { glm::vec2(m_pixelsPerUnit), 1.0f });
            return view;
        }
        inline SDL_Rect ConvertRect(const RenRect& rect, const glm::mat4& pv) override
        {
            glm::vec2 new_pos = pv * glm::vec4(rect.x, rect.y, 0.0f, 1.0f);
            glm::vec2 new_size{ rect.w * m_pixelsPerUnit.x, rect.h * m_pixelsPerUnit.y };
            return { (int)new_pos.x, (int)new_pos.y, (int)new_size.x, (int)new_size.y };
        }
    };

    class CartesianCamera : public Camera
    {
    public:
        glm::mat4 GetPV() override
        {
            // Get camera size in units.
            glm::vec2 size = GetSize();

            glm::mat4 view(1.0f);
            view = glm::translate(view, { -m_CamPos, 0.0f });

            float l = -size.x * 0.5f, r = size.x * 0.5f, b = -size.y * 0.5f, t = size.y * 0.5f, w = (float)m_viewportSize.x, h = (float)m_viewportSize.y;
            glm::mat4 proj(1.0f);
            proj = glm::translate(proj, { l * w / (l - r), t * h / (t - b), 0.0f });
            proj = glm::scale(proj, { w / (r - l), h / (b - t), 1.0f });

            return proj * view;
        } 

        // Convert rectangle defined in unit-space to window-space.
        SDL_Rect ConvertRect(const RenRect& rect, const glm::mat4& pv) override
        {
            glm::vec2 new_pos { pv * glm::vec4(rect.x, rect.y, 0.0f, 1.0f) };
            glm::vec2 new_size{ rect.w * m_pixelsPerUnit.x, rect.h * m_pixelsPerUnit.y };

            return { (int)new_pos.x, (int)(new_pos.y - new_size.y), (int)new_size.x, (int)new_size.y };
        }
        inline glm::vec2 UpDir() const { return { 0.0f, 1.0f }; }
        inline glm::vec2 RightDir() const { return { 1.0f, 0.0f }; }
    };
} // namespace Ren
