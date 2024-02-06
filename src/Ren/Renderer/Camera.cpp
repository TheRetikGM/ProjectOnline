/**
 * @brief Implementation of various cameras
 * @file Ren/Renderer/Camera.cpp
 */
#include "Ren/Renderer/Camera.hpp"

using namespace Ren;


void Camera::SetUnitScale(uint32_t pixels_per_unit) {
    if (pixels_per_unit > 0)
        m_pixelsPerUnit = glm::ivec2(pixels_per_unit);
}

void Camera::SetUnitScale(glm::ivec2 pixels_per_unit) {
    if (pixels_per_unit.x > 0 && pixels_per_unit.y > 0)
        m_pixelsPerUnit = pixels_per_unit;
}

glm::vec2 Camera::ToPixels(glm::vec2 pos, glm::mat4* pv) {
    return glm::vec2((pv ? *pv : GetPV()) * glm::vec4(pos, 0.0f, 1.0f));
}

glm::vec2 Camera::ToUnits(glm::vec2 pos, glm::mat4* pv) {
    return glm::vec2(glm::inverse(pv ? *pv : GetPV()) * glm::vec4(pos, 0.0f, 1.0f));
}

glm::vec2 Camera::ToUnitsPre(glm::vec2 pos, const glm::mat4& inv_pv) {
    return glm::vec2(inv_pv * glm::vec4(pos, 0.0f, 1.0f));
}

glm::mat4 PixelCamera::GetPV() {
    glm::mat4 view(1.0f);
    view = glm::translate(view, { -m_CamPos, 0.0f });
    view = glm::scale(view, { glm::vec2(m_pixelsPerUnit), 1.0f });
    return view;
}

SDL_Rect PixelCamera::ConvertRect(const Rect& rect, const glm::mat4& pv) {
    glm::vec2 new_pos = pv * glm::vec4(rect.x, rect.y, 0.0f, 1.0f);
    glm::vec2 new_size{ rect.w * m_pixelsPerUnit.x, rect.h * m_pixelsPerUnit.y };
    return { (int)new_pos.x, (int)new_pos.y, (int)new_size.x, (int)new_size.y };

}

glm::mat4 CartesianCamera::GetPV() {
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
    // Because idk how to create raw matrix in GLM, we use scale and translation composition to create the matrix below.
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

SDL_Rect CartesianCamera::ConvertRect(const Rect& rect, const glm::mat4& pv) {
    // Just apply the matrix to get a corrent position and scale the size using the unit scale.
    glm::vec2 new_pos { pv * glm::vec4(rect.x, rect.y, 0.0f, 1.0f) };
    glm::vec2 new_size{ rect.w * m_pixelsPerUnit.x, rect.h * m_pixelsPerUnit.y };

    // Also we need to subtract height from pos, because in pixel-space position of rectangle is represented by
    // top-left corner (in cartesian it is bottom left corder).
    return { (int)new_pos.x, (int)(new_pos.y - new_size.y), (int)new_size.x, (int)new_size.y };
}
