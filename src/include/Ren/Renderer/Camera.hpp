/**
 * @file Ren/Renderer/Camera.hpp
 * @brief Declaration of Camera classes used during rendering.
 */
#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
extern "C" {
    #include <SDL.h>
}

#include "Ren/Core/Core.hpp"

namespace Ren {
    // Represents interface for different viewing spaces and provides transformations from them to viewport's pixel-space.
    class Camera {
    public:
        virtual ~Camera() {}

        // Camera position in unit space.
        glm::vec2 m_CamPos{ .0f, .0f };

        // Return matrix for transformation from unit-space to camera-space to pixel-space (projection * view).
        virtual glm::mat4 GetPV() = 0;
        // Apply PV matrix on rect and return pixel-space SDL rectangle.
        virtual SDL_Rect ConvertRect(const Rect& rect, const glm::mat4& pv) = 0;
        // Get up direction for camera's space.
        virtual glm::vec2 UpDir() const = 0;
        // Get right direction for camera's space.
        virtual glm::vec2 RightDir() const = 0;

        // Set viewport size in pixels. Commonly this is a size of a game viewport (fullscreen -> window size).
        inline void SetViewportSize(const glm::ivec2& size) { m_viewportSize = size; }
        // How many pixels represents a single unit.
        void SetUnitScale(uint32_t pixels_per_unit);
        // How many pixels represents a single unit (with and height).
        void SetUnitScale(glm::ivec2 pixels_per_unit);
        // Returns how many pixels represents a single unit.
        inline const glm::ivec2& GetUnitScale() { return m_pixelsPerUnit; }
        // See above.
        inline SDL_Rect ConvertRect(const Rect& rect) { return ConvertRect(rect, GetPV()); };
        // Get size of camera in units.
        inline glm::vec2 GetSize() { return glm::vec2(m_viewportSize) / glm::vec2(m_pixelsPerUnit); }
        // Convert unit-space pos to pixels on viewport (taking into account camera position).
        glm::vec2 ToPixels(glm::vec2 pos, glm::mat4* pv = nullptr);
        // Convert position on viewport into position in unit-space (taking into account camera position).
        glm::vec2 ToUnits(glm::vec2 pos, glm::mat4* pv = nullptr);
        // Same as ToUnits(), but with precaculated inverse PV matrix.
        glm::vec2 ToUnitsPre(glm::vec2 pos, const glm::mat4& inv_pv);
    protected:
        // Size of viewport (window) in pixels.
        glm::ivec2 m_viewportSize{ 1, 1 };
        glm::ivec2 m_pixelsPerUnit{ 1, 1 };
    };

    // Basic camera used for the default SDL pixel space. (camera position is left top cornet)
    // Allows for camera position and scaling.
    class PixelCamera : public Camera {
        inline glm::vec2 UpDir() const override { return { 0.0f, -1.0f }; }
        inline glm::vec2 RightDir() const override { return { 1.0f, 0.0f }; }
        glm::mat4 GetPV() override;
        SDL_Rect ConvertRect(const Rect& rect, const glm::mat4& pv) override;
    };

    // Camera used for converting from cartesian-space into pixel space.
    // It is needed to provide camera position in cartesian-space, unit scale and viewport size to correcly scale things.
    class CartesianCamera : public Camera {
    public:
        glm::mat4 GetPV() override;

        // Convert rectangle defined in unit-space to window-space.
        // FIXME: maybe position of rectangle should be the center of rectangle?
        SDL_Rect ConvertRect(const Rect& rect, const glm::mat4& pv) override;
        inline glm::vec2 UpDir() const override { return { 0.0f, 1.0f }; }
        inline glm::vec2 RightDir() const override { return { 1.0f, 0.0f }; }
    };
} // namespace Ren
