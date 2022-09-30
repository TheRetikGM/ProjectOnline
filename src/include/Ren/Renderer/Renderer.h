#pragma once
#include <vector>
#include <algorithm>

#include "RenderCommand.hpp"
#include "Ren/Renderer/Camera.h"
#include "Ren/RenSDL/Texture.h"

namespace Ren
{
    // Class which is used for centralized rendering across the engine.
    // Submit RenderCommands, that imeplents the needs of RenderCommand (implements needed functions. For more info check RenderCommand.hpp)
    class Renderer
    {
    public:
        // Use this function on the start of render phase.
        // TODO: Maybe submit render texture target here?
        static void BeginRender(Camera* camera, Texture2D* render_target = nullptr)
        {
            m_renderCommands.clear();
            m_camera = camera;
            m_cameraPV = m_camera->GetPV();
            m_cameraInvPV = glm::inverse(m_cameraPV);
            m_renderTarget = render_target;

            if (render_target)
                SDL_SetRenderTarget(m_renderer, render_target->m_Texture);
        }
        static void EndRender()
        {
            if (m_renderTarget)
                SDL_SetRenderTarget(m_renderer, nullptr);
            m_renderTarget = nullptr;
        }

        // Submit raw RenderCommands, to be rendered.
        template<typename T>
        static void SubmitCommand(T comm) { m_renderCommands.push_back(comm); }

        // Use these calls to render. They take into account camera and stuff.
        inline static void SetRenderLayer(int32_t layer) { m_activeRenderLayer = layer; }
        static void RenderQuad(const Ren::Rect& rect, float rotation_deg, const Ren::Color4& color);
        static void RenderQuad(const Ren::Rect& rect, float rotation_deg, const Ren::Color3& color, SDL_Texture* texture);
        static void DrawRect(const Ren::Rect& rect, float rotation_deg, const Ren::Color4& color);
        static void DrawCircle(const Ren::Rect& rect, const Ren::Color4& color, uint32_t precision = 32);
        static void DrawCircle(const glm::vec2& pos, float radius, const Ren::Color4& color, uint32_t precision = 32);
        static void DrawLine(const glm::vec2& p1, const glm::vec2& p2, const Ren::Color4& color);

        // Executes all render commands, that were submitted earlier.
        // TODO: Rendering optimizations like culling
        static void Render()
        {
            // Sort by layer, so that the commands are executed in correct order. For commands in same layer, preserve the order they were submitted in.
            std::stable_sort(m_renderCommands.begin(), m_renderCommands.end(), [](const RenderCommand& a, const RenderCommand& b) { return a->GetLayer() < b->GetLayer(); });
            for (auto&& command : m_renderCommands)
                command->Render(m_renderer);
        }

        // Clear current render target.
        static void Clear(Ren::Color4 color = Ren::Colors4::Black);
        inline static SDL_Renderer* GetRenderer() { return m_renderer; } 
        inline static void SetRenderer(SDL_Renderer* renderer) { m_renderer = renderer; }
        inline static SDL_Rect ConvertRect(const Ren::Rect& rect) { return m_camera->ConvertRect(rect, m_cameraPV); }
        inline static Camera* GetCamera() { return m_camera; }
        // Get position in the current viewport in respect to the camera.
        inline static glm::vec2 ToPixels(glm::vec2 point) { return m_camera->ToPixels(point, &m_cameraPV); }

    private:
        inline static std::vector<RenderCommand> m_renderCommands{};
        inline static SDL_Renderer* m_renderer{ nullptr };
        inline static int32_t m_activeRenderLayer{ 0 };
        inline static Ren::Texture2D* m_renderTarget{ nullptr };

        inline static Camera* m_camera{ nullptr };
        inline static glm::mat4 m_cameraPV{ 1.0f };
        inline static glm::mat4 m_cameraInvPV{ 1.0f };
    };

    inline static glm::vec2 UpDir() { return Renderer::GetCamera()->UpDir(); }
    inline static glm::vec2 RightDir() { return Renderer::GetCamera()->RightDir(); }
} // namespace Ren
