#pragma once
#include <vector>
#include <algorithm>

#include "RenderCommand.hpp"

namespace Ren
{
    class Renderer
    {
    public:
        static void BeginRender()
        {
            m_renderCommands.clear();
        }

        template<typename T>
        static void SubmitCommand(T comm) { m_renderCommands.push_back(comm); }

        static void Render(SDL_Renderer* renderer)
        {
            // Sort by layer.
            std::stable_sort(m_renderCommands.begin(), m_renderCommands.end(), [](const RenderCommand& a, const RenderCommand& b) { return a->GetLayer() < b->GetLayer(); });
            for (auto&& command : m_renderCommands)
                command->Render(renderer);
        }


    private:
        inline static std::vector<RenderCommand> m_renderCommands{};
    };
} // namespace Ren
