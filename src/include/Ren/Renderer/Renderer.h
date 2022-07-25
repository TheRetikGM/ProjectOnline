#pragma once
#include <vector>
#include <algorithm>

#include "RenderCommand.hpp"
#include "Ren/Core/Camera.h"

namespace Ren
{
    // Class which is used for centralized rendering across the engine.
    // Submit RenderCommands, that imeplents the needs of RenderCommand (implements needed functions. For more info check RenderCommand.hpp)
    class Renderer
    {
    public:
        // Use this function on the start of render phase.
        // TODO: Maybe submit a camera and render texture target here?
        static void BeginRender(Camera* camera)
        {
            m_renderCommands.clear();
            m_camera = camera;
            m_cameraPV = m_camera->GetPV();
        }

        // Submit raw RenderCommands, to be rendered.
        template<typename T>
        static void SubmitCommand(T comm) { m_renderCommands.push_back(comm); }

        // Use these calls to render. They take into account camera and stuff.
        inline static void SetRenderLayer(int32_t layer) { m_activeRenderLayer = layer; }
        static void RenderQuad(const Ren::Rect& rect, float rotation, const Ren::Color4& color);
        static void RenderQuad(const Ren::Rect& rect, float rotation, const Ren::Color3& color, SDL_Texture* texture);
        static void DrawRect(const Ren::Rect& rect, float rotation, const Ren::Color4& color);
        static void DrawCircle(const Ren::Rect& rect, const Ren::Color4& color, uint32_t precision = 32);
        static void DrawCircle(const glm::vec2& pos, float radius, const Ren::Color4& color, uint32_t precision = 32);

        // Executes all render commands, that were submitted earlier.
        // TODO: Rendering optimizations like culling
        static void Render()
        {
            // Sort by layer, so that the commands are executed in correct order. For commands in same layer, preserve the order they were submitted in.
            std::stable_sort(m_renderCommands.begin(), m_renderCommands.end(), [](const RenderCommand& a, const RenderCommand& b) { return a->GetLayer() < b->GetLayer(); });
            for (auto&& command : m_renderCommands)
                command->Render(m_renderer);
        }

        inline static SDL_Renderer* GetRenderer() { return m_renderer; } 
        inline static void SetRenderer(SDL_Renderer* renderer) { m_renderer = renderer; }
        inline static SDL_Rect ConvertRect(const Ren::Rect& rect) { return m_camera->ConvertRect(rect, m_cameraPV); }
        inline static Camera* GetCamera() { return m_camera; }
        // Get position in the current viewport in respect to the camera.
        // FIXME: Think of a better name...
        inline glm::vec2 ToVpSpace(glm::vec2 point) { return glm::vec2(m_cameraPV * glm::vec4(point, 0.0f, 1.0f)); }

    private:
        inline static std::vector<RenderCommand> m_renderCommands{};
        inline static SDL_Renderer* m_renderer{ nullptr };
        inline static int32_t m_activeRenderLayer{ 0 };

        inline static Camera* m_camera{ nullptr };
        inline static glm::mat4 m_cameraPV{ 1.0f };
    };

    inline static glm::vec2 UpDir() { return Renderer::GetCamera()->UpDir(); }
    inline static glm::vec2 RightDir() { return Renderer::GetCamera()->RightDir(); }
} // namespace Ren
