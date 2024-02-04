/**
 * @file Ren/ECS/Loaders.hpp
 * @brief Declaration of ENTT loaders.
 *
 * For example Texture loader loads a texture and then the ENTT has it in cache. And it will not be loaded from disk again until the cache is not cleared.
 */
#pragma once
extern "C" {
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_ttf.h>
}
#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace Ren {
    struct TextureResource {
        SDL_Texture* texture{ nullptr };
        glm::ivec2 size;

        TextureResource(SDL_Texture* tex, glm::ivec2 size)
            : texture(tex), size(size) {}
    };

    // Manages loading of texture resources using SDL_Image as a loader.
    class TextureLoader {
    public:
        using result_type = std::shared_ptr<TextureResource>;

        static void TextureDeleter(TextureResource* texture);

        // Loader for sprite to be loaded from disk.
        result_type operator()(SDL_Renderer* renderer, std::string path_to_texture);

        // Loader for texture, which is generated from some text.
        result_type operator()(SDL_Renderer* renderer, TTF_Font* font, std::string text, glm::ivec4 color);
    };
    using TextureCache = entt::resource_cache<TextureResource, TextureLoader>;
    using TextureHandle = std::pair<entt::id_type, entt::resource<TextureResource>>;
} // namespace Ren

