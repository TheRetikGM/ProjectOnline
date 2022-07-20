#include "Ren/Renderer/Renderer.h"

using namespace Ren;

struct Transform
{
    glm::vec2 position{ 0.0f, 0.0f };
    glm::vec2 size{ 1.0f, 1.0f };
    float rotation{ 0.0f };

    Transform() = default;
    inline SDL_Rect ToRect() { return Renderer::ConvertRect({ position.x, position.y, size.x, size.y }); };
};

struct QuadRenderCommand
{
    struct QuadInfo
    {
        Transform trans{};
        SDL_Texture* texture{ nullptr };
        Color4 color{ Colors4::White };
        QuadInfo() = default;
        QuadInfo(const glm::vec2& p, const glm::vec2& s, float r, SDL_Texture* tex, Color4 color) : trans({ p, s, r }), texture(tex), color(color) {}
    };

    int32_t layer = 0;
    QuadInfo info{};

    QuadRenderCommand(const QuadInfo& quad_info, int32_t layer)
        : layer(layer)
        , info(quad_info) {}

    inline int32_t GetLayer() const { return layer; }
    void Render(SDL_Renderer* renderer)
    {
        SDL_Rect rect = info.trans.ToRect();
        if (info.texture)
        {
            SDL_SetTextureColorMod(info.texture, info.color.r, info.color.g, info.color.b );
            SDL_RenderCopyEx(renderer, info.texture, nullptr, &rect, info.trans.rotation, nullptr, {});
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, info.color.r, info.color.g, info.color.b, info.color.a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};

struct RectRenderCommand
{
    Transform trans{};
    Color4 color{ Colors4::White };
    int32_t layer{ 0 };

    RectRenderCommand(const Transform& t, Color4 color, int32_t layer) : trans(t), color(color), layer(layer) {}
    int32_t GetLayer() const { return layer; }
    inline void Render(SDL_Renderer* renderer)
    {
        SDL_Rect rect = trans.ToRect();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawRect(renderer, &rect);
    }
};


void Renderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ren::Color4& color)
{
    SubmitCommand(QuadRenderCommand({ pos, size, rotation, nullptr, color }, m_activeRenderLayer));
}
void Renderer::SubmitQuad(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ren::Color3& color, SDL_Texture* texture)
{
    SubmitCommand(QuadRenderCommand({ pos, size, rotation, texture, Color4(color, 255) }, m_activeRenderLayer));
}
void Renderer::SubmitRect(const glm::vec2& pos, const glm::vec2& size, float rotation, const Ren::Color4& color)
{
    SubmitCommand(RectRenderCommand({ pos, size, rotation }, color, m_activeRenderLayer));
}