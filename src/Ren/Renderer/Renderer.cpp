#define _USE_MATH_DEFINES
#include <cmath>
#include "Ren/Renderer/Renderer.h"

using namespace Ren;

const float TAU = 2.0f * float(M_PI);

struct Transform
{
    glm::vec2 position{ 0.0f, 0.0f };
    glm::vec2 size{ 1.0f, 1.0f };
    float rotation{ 0.0f };

    Transform() = default;
    inline SDL_Rect ToRect() { return Renderer::ConvertRect({ position.x, position.y, size.x, size.y }); };
};

#pragma region Render commands

// We use specialized render commands, which use camera to render.
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
            SDL_RenderCopyEx(renderer, info.texture, nullptr, &rect, -info.trans.rotation, nullptr, {});
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
    inline int32_t GetLayer() const { return layer; }
    void Render(SDL_Renderer* renderer)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        if (trans.rotation == 0.0f)
        {
            SDL_Rect rect = trans.ToRect();
            SDL_RenderDrawRect(renderer, &rect);
            return;
        }

        // Rotate the rectangle corners, convert them to pixel-space and render them.
        std::array<glm::vec2, 5> points;
        points[0] = { trans.position.x, trans.position.y };
        points[1] = { trans.position.x + trans.size.x, trans.position.y };
        points[2] = { trans.position.x + trans.size.x, trans.position.y + trans.size.y };
        points[3] = { trans.position.x + 0.0f, trans.position.y + trans.size.y };
        points[4] = { trans.position.x, trans.position.y };
        float sinA = std::sin(glm::radians(trans.rotation));
        float cosA = std::cos(glm::radians(trans.rotation));
        for (auto&& p : points) {
            p -= trans.position + trans.size / 2.0f;
            p = { p.x * cosA - p.y * sinA, 
                  p.x * sinA + p.y * cosA };
            p += trans.position + trans.size / 2.0f;
            p = Renderer::ToPixels(p);
        }

        SDL_RenderDrawLinesF(renderer, (SDL_FPoint*)points.data(), (int)points.size());
    }
};

struct CircleRenderCommand
{
    Transform trans{};
    Color4 color{ Colors4::White };
    uint32_t precision{ 32 };
    bool fill = false;
    int32_t layer{ 0 };

    CircleRenderCommand(const Transform& t, Color4 color, uint32_t precision, bool fill, int32_t layer) : trans(t), color(color), precision(precision), fill(fill), layer(layer) {}
    inline int32_t GetLayer() const { return layer; }
    void Render(SDL_Renderer* renderer)
    {
        REN_ASSERT(fill == false, "Fill circle render command is not implemented yet.");

        SDL_Rect rect = trans.ToRect();
        float x = rect.x + rect.w / 2.0f, y = rect.y + rect.h / 2.0f;
        
        // Draw 'precision' number of lines, using points on the circle.
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        float step_size = TAU / float(precision);
        for (float angle = 0.0f; angle < TAU; angle += step_size)
        {
            glm::vec2 p1 = glm::vec2(std::cos(angle) * rect.w * 0.5f + x, std::sin(angle) * rect.h * 0.5f + y);
            glm::vec2 p2 = glm::vec2(std::cos(angle + step_size) * rect.w * 0.5f + x, std::sin(angle + step_size) * rect.h * 0.5f + y);
            SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
        }
    }
};
struct LineRenderCommand
{
    glm::vec2 p1, p2;
    Color4 color;
    int32_t layer;
    const int32_t GetLayer() const { return layer; }
    void Render(SDL_Renderer* renderer)
    {
        p1 = Renderer::ToPixels(p1);
        p2 = Renderer::ToPixels(p2);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLineF(renderer, p1.x, p1.y, p2.x, p2.y);
    }
};

#pragma endregion
#pragma region Submission wrappers.

void Renderer::RenderQuad(const Ren::Rect& rect, float rotation, const Ren::Color4& color)
{
    SubmitCommand(QuadRenderCommand({ rect.pos, rect.size, rotation, nullptr, color }, m_activeRenderLayer));
}
void Renderer::RenderQuad(const Ren::Rect& rect, float rotation, const Ren::Color3& color, SDL_Texture* texture)
{
    SubmitCommand(QuadRenderCommand({ rect.pos, rect.size, rotation, texture, Color4(color, 255) }, m_activeRenderLayer));
}
void Renderer::DrawRect(const Ren::Rect& rect, float rotation, const Ren::Color4& color)
{
    SubmitCommand(RectRenderCommand({ rect.pos, rect.size, rotation }, color, m_activeRenderLayer));
}
void Renderer::DrawCircle(const Ren::Rect& rect, const Ren::Color4& color, uint32_t precision)
{
    SubmitCommand(CircleRenderCommand({ rect.pos, rect.size, 0.0f }, color, precision, false, m_activeRenderLayer));
}
void Renderer::DrawCircle(const glm::vec2& pos, float radius, const Ren::Color4& color, uint32_t precision)
{
    SubmitCommand(CircleRenderCommand({ pos - glm::vec2(radius), glm::vec2(radius) * 2.0f, 0.0f }, color, precision, false, m_activeRenderLayer));
}
void Renderer::DrawLine(const glm::vec2& p1, const glm::vec2& p2, const Ren::Color4& color)
{
    SubmitCommand(LineRenderCommand{ p1, p2, color, m_activeRenderLayer });
}

#pragma endregion


void Renderer::Clear(Ren::Color4 color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(m_renderer);
}