/**
 * @brief Implementation of ECS entt loaders.
 * @file Ren/ECS/Loaders.cpp
 */
#include "Ren/ECS/Loaders.hpp"
#include "Ren/Core/Core.hpp"
#include "Ren/Core/AssetManager.hpp"

using namespace Ren;


void TextureLoader::TextureDeleter(TextureResource* texture) {
    SDL_DestroyTexture(texture->texture);
    delete texture;
};

TextureLoader::result_type TextureLoader::operator()(SDL_Renderer* renderer, std::string path_to_texture) {
    // Load texture and get it's size.
    SDL_Surface* texture_surface = IMG_Load(AssetManager::GetImage(path_to_texture).string().c_str());
    REN_ASSERT(texture_surface != nullptr, "Failed to load texture. Error: " + std::string(IMG_GetError()));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, texture_surface);
    REN_ASSERT(texture != nullptr, "Failed to create texture from surface. Error: " + std::string(IMG_GetError()));
    glm::ivec2 texture_size{ texture_surface->w, texture_surface->h };
    SDL_FreeSurface(texture_surface);

    // Shared_ptr with custom deleter, which properly deletes the texture.
    return std::shared_ptr<TextureResource>(new TextureResource(texture, texture_size), TextureDeleter);
}

TextureLoader::result_type TextureLoader::operator()(SDL_Renderer* renderer, TTF_Font* font, std::string text, glm::ivec4 color) {
    auto text_surface{ TTF_RenderText_Solid(font, AssetManager::GetFont(text).string().c_str(), { (uint8_t)color.r, (uint8_t)color.g, (uint8_t)color.b, (uint8_t)color.a }) };
    REN_ASSERT(text_surface != nullptr, "Cannot generate surface from text! Error: " + std::string(TTF_GetError()));
    glm::ivec2 size{ text_surface->w, text_surface->h };
    auto texture{ SDL_CreateTextureFromSurface(renderer, text_surface) };
    SDL_FreeSurface(text_surface);

    return std::shared_ptr<TextureResource>(new TextureResource(texture, size), TextureDeleter);
}

