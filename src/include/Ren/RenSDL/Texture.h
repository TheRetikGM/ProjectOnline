#pragma once
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <Ren/Core/Core.h>

namespace Ren
{
    class Texture2D
    {
    public:
        glm::ivec2 m_Size{ 512, 512 };
        Uint32 m_Format{ SDL_PIXELFORMAT_RGBA32 };
        int m_Access{ SDL_TEXTUREACCESS_STATIC };
        SDL_Texture* m_Texture{ nullptr };

        void Generate(void* data = nullptr);

        // Set which color should be discarded when rendering.
        inline void SetColorKey(Uint32 sdl_color)
        {
            m_hasColorKey = true;
            m_colorKey = sdl_color;
        }
        inline void UnsetColorKey() { m_hasColorKey = false; }

    private:
        bool m_hasColorKey = false;
        Uint32 m_colorKey{ 0 };
    };
}