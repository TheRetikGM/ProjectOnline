#pragma once
extern "C" {
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL_ttf.h>
}
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "Ren/Core/Core.h"

namespace Ren
{
    struct TextureResource 
    { 
        SDL_Texture* texture{ nullptr }; 
        glm::ivec2 size;

        TextureResource(SDL_Texture* tex, glm::ivec2 size)
            : texture(tex), size(size) {}
    };
    
    // Manages loading of texture resources using SDL_Image as a loader.
    class TextureLoader
    {
    public:
        using result_type = std::shared_ptr<TextureResource>;
        
        static void TextureDeleter(TextureResource* texture) 
        {
            SDL_DestroyTexture(texture->texture);
            delete texture;
        };

        // Loader for sprite to be loaded from disk.
        result_type operator()(SDL_Renderer* renderer, std::string path_to_texture)
        {
            // Load texture and get it's size.
            SDL_Surface* texture_surface = IMG_Load(path_to_texture.c_str());
            REN_ASSERT(texture_surface != nullptr, "Failed to load texture. Error: " + std::string(IMG_GetError()));
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, texture_surface);
            REN_ASSERT(texture != nullptr, "Failed to create texture from surface. Error: " + std::string(IMG_GetError()));
            glm::ivec2 texture_size{ texture_surface->w, texture_surface->h };
            SDL_FreeSurface(texture_surface);

            // Shared_ptr with custom deleter, which properly deletes the texture.
            return std::shared_ptr<TextureResource>(new TextureResource(texture, texture_size), TextureDeleter);
        }

        // Loader for texture, which is generated from some text.
        result_type operator()(SDL_Renderer* renderer, TTF_Font* font, std::string text, glm::ivec4 color)
        {
            auto text_surface{ TTF_RenderText_Solid(font, text.c_str(), { (uint8_t)color.r, (uint8_t)color.g, (uint8_t)color.b, (uint8_t)color.a }) };
            REN_ASSERT(text_surface != nullptr, "Cannot generate surface from text! Error: " + std::string(TTF_GetError()));
            glm::ivec2 size{ text_surface->w, text_surface->h };
            auto texture{ SDL_CreateTextureFromSurface(renderer, text_surface) };
            SDL_FreeSurface(text_surface);

            return std::shared_ptr<TextureResource>(new TextureResource(texture, size), TextureDeleter);
        }
    };
    using TextureCache = entt::resource_cache<TextureResource, TextureLoader>;
    using TextureHandle = std::pair<entt::id_type, entt::resource<TextureResource>>;
} // namespace Ren

