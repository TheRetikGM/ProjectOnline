#include "Ren/Renderer/TextRenderer.h"
#include <iostream>
#include <exception>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Ren/Renderer/Renderer.h"
#include "Ren/RenSDL/Texture.hpp"

using namespace Ren;

struct CharacterRenderCommand
{
    glm::ivec2 pos, size;
    glm::ivec3 color;
    Character& ch;
    int32_t layer;

    CharacterRenderCommand(glm::ivec2 pos, glm::vec2 size, glm::ivec3 color, Character& c, int32_t layer)
        : pos(pos), size(size), color(color), ch(c), layer(layer)
    {
    }

    inline int GetLayer() const { return layer; }

    void Render(SDL_Renderer* renderer)
    {
        if (ch.has_texture)
        {
            SDL_Rect rect{ pos.x, pos.y, size.x, size.y };
            SDL_SetTextureColorMod(ch.texture.m_Texture, color.r, color.g, color.b);
            SDL_RenderCopy(renderer, ch.texture.m_Texture, nullptr, &rect);
        }
    }
};


TextRenderer::TextRenderer() {}
TextRenderer::~TextRenderer()
{
    for (auto&& [c, ch]  : m_Characters)
        if (ch.has_texture)
            SDL_DestroyTexture(ch.texture.m_Texture);
    this->m_Characters.clear();
}

void TextRenderer::Load(std::string font, unsigned int font_size)
{
    this->m_Characters.clear();

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        throw std::runtime_error("Could not init FreeType Library.");
    
    FT_Face face;
    if(FT_New_Face(ft, font.c_str(), 0, &face))
        throw std::runtime_error("Failed to load font '" + font + "'.");
    
    FT_Set_Pixel_Sizes(face, 0, font_size);

    for (uint32_t c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            REN_STATUS("Failed to load Glyph. C = " + std::to_string(c));
            continue;
        }

        Texture2D tex;
        bool has_texture = false;

        if (face->glyph->bitmap.width != 0 && face->glyph->bitmap.rows != 0)
        {
            tex.m_Format = SDL_PIXELFORMAT_RGBA32;
            tex.m_Size = { face->glyph->bitmap.width, face->glyph->bitmap.rows };
            uint8_t* tex_data = new uint8_t[tex.m_Size.x * tex.m_Size.y * 4];
            
            // Glyph buffer has only one channel, so we need to create RGBA texture
            // which has all the required channels. Also, the 1 channel in FreeType
            // represents transparency, so we set it as Alpha and all the other 
            // channels will be 255, so that we can easily set color later.
            for (uint32_t i = 0; i < tex.m_Size.x * tex.m_Size.y; i++)
            {
                std::memset(&tex_data[i * 4], 255, 3);
                tex_data[i * 4 + 3] = face->glyph->bitmap.buffer[i];

            }
            tex.Generate(tex_data);

            delete[] tex_data;
            has_texture = true;
        }

        // Store character for later use
        Character character = {
            tex, has_texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        m_Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    this->m_fontSize = font_size;
    this->m_RowSpacing = int(0.5f * this->m_Characters.at('H').size.y);
}
void TextRenderer::RenderText(std::string text, glm::vec2 pos, float scale, Color3 color, int32_t layer)
{
    float x_orig = pos.x;

    std::string::iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        if (*c == '\n')
        {
            pos.x = x_orig;
            pos.y += (m_Characters.at('H').size.y + m_RowSpacing) * scale;
            continue;
        }

        Character& ch = m_Characters.at(*c);

        // FIXME: Spaces are transparent for now. In the future we could issue a different render command to render rectangles.
        if (ch.has_texture)
        {
            float xpos = pos.x + ch.bearing.x * scale;
            float ypos = pos.y + (m_Characters.at('H').bearing.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            Renderer::SubmitCommand(CharacterRenderCommand({ (int)xpos, (int)ypos }, { (int)w, (int)h }, glm::ivec3(color), ch, layer));
        }

        pos.x += (ch.advance >> 6) * scale;
    }
}

glm::ivec2 TextRenderer::GetStringSize(std::string str, float scale) const
{
    glm::ivec2 size(0, 0);
    size_t len = str.length();
    for (int i = 0; i < int(len); i++)
    {
        size.x += m_Characters.at(str[i]).advance >> 6;
        size.y = std::max(size.y, m_Characters.at(str[i]).size.y);
    }

    return size;
}