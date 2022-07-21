#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL.h>

#include "Ren/Core/Core.h"
#include "Ren/RenSDL/Context.hpp"

namespace Ren
{
    // Rectangle with float precision.
    struct RenRect { float x, y, w, h; };

    // Represents interface for different viewing spaces and provides transformations from them to viewport's pixel-space.
    class Camera
    {
    public:
        // Camera position in unit space.
        glm::vec2 m_CamPos{ .0f, .0f };

        // Return matrix for transformation from unit-space to camera-space to pixel-space (projection * view).
        virtual glm::mat4 GetPV() = 0;
        // Apply PV matrix on rect and return pixel-space SDL rectangle.
        virtual SDL_Rect ConvertRect(const RenRect& rect, const glm::mat4& pv) = 0;
        // Get up direction for camera's space.
        virtual glm::vec2 UpDir() const = 0;
        // Get right direction for camera's space.
        virtual glm::vec2 RightDir() const = 0;

        // Set viewport size in pixels. Commonly this is a size of a game viewport (fullscreen -> window size).
        inline void SetViewportSize(const glm::ivec2& size) { m_viewportSize = size; }
        // How many pixels represents a single unit.
        inline void SetUnitScale(uint32_t pixels_per_unit) { m_pixelsPerUnit = glm::ivec2(pixels_per_unit); }
        // How many pixels represents a single unit (with and height).
        inline void SetUnitScale(glm::ivec2 pixels_per_unit) { m_pixelsPerUnit = pixels_per_unit; }
        // Returns how many pixels represents a single unnit.
        inline const glm::ivec2& GetUnitScale() { return m_pixelsPerUnit; }
        // See above.
        inline SDL_Rect ConvertRect(const RenRect& rect) { return ConvertRect(rect, GetPV()); };
        // Get size of camera in units.
        inline glm::vec2 GetSize() { return glm::vec2(m_viewportSize) / glm::vec2(m_pixelsPerUnit); }
    protected:
        // Size of viewport (window) in pixels.
        glm::ivec2 m_viewportSize{ 1, 1 };
        glm::ivec2 m_pixelsPerUnit{ 1, 1 };
    };

    // Basic camera used for the default SDL pixel space. (camera position is left top cornet)
    // Allows for camera position and scaling.
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

    // Camera used for converting from cartesian-space into pixel space.
    // It is needed to provide camera position in cartesian-space, unit scale and viewport size to correcly scale things.
    class CartesianCamera : public Camera
    {
    public:
        glm::mat4 GetPV() override
        {
            // Get camera size in units.
            glm::vec2 size = GetSize();

            // Move all things in oposite direction of camera pos, to simulate the camera moving.
            // In the future we vould apply camera rotation here if needed.
            // View matrix moves vertices to camera-space.
            glm::mat4 view(1.0f);
            view = glm::translate(view, { -m_CamPos, 0.0f });

            // Create a projection matrix, which converts from camera-sapce to viewport's pixel space.
            // TLDR; Projects vertices in the range of camera size from camera-space onto the viewport.
            // For the math check the matrix below.  Here it is just scaled to use with 3D vectors (with z = 0), because GLM doesn't support matrix transformations for 2D vectors.
            // Because idk of to create raw matrix in GLM, we use scale and translation composition to create the matrix below.
            float l = -size.x * 0.5f, r = size.x * 0.5f, b = -size.y * 0.5f, t = size.y * 0.5f, w = (float)m_viewportSize.x, h = (float)m_viewportSize.y;
            glm::mat4 proj(1.0f);
            proj = glm::translate(proj, { l * w / (l - r), t * h / (t - b), 0.0f });
            proj = glm::scale(proj, { w / (r - l), h / (b - t), 1.0f });
            
            /* Projection matrix = 
                [ w/(r-l)     0     l*w/(l-r) ]
                [    0     h/(b-t)  t*h/(t-b) ]
                [    0        0         1     ]

              - l ... left of camera (relative to camera position) in units
              - r ... right of camera in units
              - t ... top of camera in units
              - b ... bottom of camera in units
              - w,h ... width and height of viewport in pixels.
            */

            return proj * view;
        } 

        // Convert rectangle defined in unit-space to window-space.
        // FIXME: maybe position of rectangle should be the center of rectangle?
        SDL_Rect ConvertRect(const RenRect& rect, const glm::mat4& pv) override
        {
            // Just apply the matrix to get a corrent position and scale the size using the unit scale.
            glm::vec2 new_pos { pv * glm::vec4(rect.x, rect.y, 0.0f, 1.0f) };
            glm::vec2 new_size{ rect.w * m_pixelsPerUnit.x, rect.h * m_pixelsPerUnit.y };

            // Also we need to subtract height from pos, because in pixel-space position of rectangle is represented by 
            // top-left corner (in cartesian it is bottom left corder).
            return { (int)new_pos.x, (int)(new_pos.y - new_size.y), (int)new_size.x, (int)new_size.y };
        }
        inline glm::vec2 UpDir() const { return { 0.0f, 1.0f }; }
        inline glm::vec2 RightDir() const { return { 1.0f, 0.0f }; }
    };
} // namespace Ren
