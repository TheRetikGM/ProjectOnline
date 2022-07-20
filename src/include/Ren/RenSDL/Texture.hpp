#pragma once
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "Ren/Core/Core.h"
#include "Ren/Renderer/Renderer.h"

namespace Ren
{
    // Takem from: https://stackoverflow.com/questions/29061505/where-can-i-get-correct-pitch-parameter-for-sdl-renderreadpixels-function-in-sdl.
    inline int SDL_CalculatePitch(Uint32 format, int width)
    {
        int pitch;

        if (SDL_ISPIXELFORMAT_FOURCC(format) || SDL_BITSPERPIXEL(format) >= 8) {
            pitch = (width * SDL_BYTESPERPIXEL(format));
        } else {
            pitch = ((width * SDL_BITSPERPIXEL(format)) + 7) / 8;
        }
        pitch = (pitch + 3) & ~3;   /* 4-byte aligning for speed */
        return pitch;
    }

    class Texture2D
    {
    public:
        glm::ivec2 m_Size{ 512, 512 };
        Uint32 m_Format{ SDL_PIXELFORMAT_RGBA32 };
        int m_Access{ SDL_TEXTUREACCESS_STATIC };
        SDL_Texture* m_Texture{ nullptr };

        inline void Generate(void* data = nullptr)
        {
            REN_ASSERT(Renderer::GetRenderer() != nullptr, "No renderer is set");

            if (data != nullptr)
            {
                int pitch = SDL_CalculatePitch(m_Format, m_Size.x);
                int n_channels = pitch / m_Size.x;
                REN_ASSERT(n_channels == 3 || n_channels == 4, "Channel count " + std::to_string(n_channels) + " is not supported.");

                Uint32 rmask, gmask, bmask, amask;

                if constexpr (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                {
                    int shift = n_channels == 3 ? 8 : 0;
                    rmask = 0xff000000 >> shift;
                    gmask = 0x00ff0000 >> shift;
                    bmask = 0x0000ff00 >> shift;
                    amask = 0x000000ff >> shift;
                }
                else    // Litle endian like x86
                {
                    rmask = 0x000000ff;
                    gmask = 0x0000ff00;
                    bmask = 0x00ff0000;
                    amask = (n_channels > 3) ? 0xff000000 : 0;
                }

                SDL_Surface* tmp_surface = SDL_CreateRGBSurfaceFrom(data, m_Size.x, m_Size.y, n_channels * 8, pitch, rmask, gmask, bmask, amask);
                REN_ASSERT(tmp_surface != nullptr, "Failed to create temporary surface. SDL_Error: " + std::string(SDL_GetError()));

                if (m_hasColorKey)
                    SDL_SetColorKey(tmp_surface, SDL_TRUE, m_colorKey);

                m_Texture = SDL_CreateTextureFromSurface(Renderer::GetRenderer(), tmp_surface);
                SDL_FreeSurface(tmp_surface);
            }
            else
                m_Texture = SDL_CreateTexture(Renderer::GetRenderer(), m_Format, m_Access, m_Size.x, m_Size.y);
            REN_ASSERT(m_Texture != nullptr, "Failed to create texture. SDL_Error: " + std::string(SDL_GetError()));
        }

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