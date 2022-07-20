#pragma once
#include <vector>
#include <algorithm>

#include "RenderCommand.hpp"

namespace Ren
{
    // Class which is used for centralized rendering across the engine.
    // Submit RenderCommands, that imeplents the needs of RenderCommand (implements needed functions. For more info check RenderCommand.hpp)
    class Renderer
    {
    public:
        // Use this function on the start of render phase.
        // TODO: Maybe submit a camera and render texture target here?
        static void BeginRender()
        {
            m_renderCommands.clear();
        }

        // Submit RenderCommands, to be rendered.
        template<typename T>
        static void SubmitCommand(T comm) { m_renderCommands.push_back(comm); }

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

    private:
        inline static std::vector<RenderCommand> m_renderCommands{};
        inline static SDL_Renderer* m_renderer{ nullptr };
    };
} // namespace Ren
