#include "renderer.hpp"

#include "SDL3/SDL_render.h"
#include "asset_manager.hpp"
#include "log.hpp"
#include "texture.hpp"
#include "utils.hpp"

Renderer::Renderer(SDL_Renderer* renderer) {
    renderer_ = renderer;
}

void Renderer::destroy() {
    SDL_DestroyRenderer(renderer_);
}

Renderer Renderer::init(SDL_Window* window) {
    SDL_Renderer* r = SDL_CreateRenderer(window, nullptr);
    if(!r) {
        std::string error = SDL_GetError();
        FATAL_ERROR("[RENDERER]: failed to init - " + error);
    }

    Renderer renderer(r);
    return renderer;
}

SDL_Renderer* Renderer::handle() {
    return renderer_;
}

void Renderer::toogleVsync(bool toggle) {
    // vsync is disabled by default
    bool result = false;
    if(toggle) {
        result = SDL_SetRenderVSync(renderer_, 1);
    } else {
        SDL_SetRenderVSync(renderer_, 0);
    }

    if(!result) {
        INFO("failed to set v-sync");
    }
}

void Renderer::queueRenderTexture(
    Strata strata, const std::string& textureName, const Rect& sRect, const Rect& dRect) {
    auto texture = AssetManager::get()->load<Texture>(textureName);

    if(!texture) {
        ERROR_ONCE("[RENDERER]: failed to acquire texture - " + textureName);
        return;
    }

    queueRenderCall(strata, [&, texture, sRect, dRect]() {
        SDL_FRect srcRect = {sRect.x, sRect.y, sRect.w, sRect.h};
        SDL_FRect dstRect = {dRect.x, dRect.y, dRect.w, dRect.h};
        SDL_RenderTexture(renderer_, texture->get(), &srcRect, &dstRect);
    });
}

void Renderer::queueRenderTextureRotated(
    Strata strata, const std::string& textureName, const Rect& sRect, const Rect& dRect,
    const Vec2& pivot, f32 angleInDegrees) {
    auto texture = AssetManager::get()->load<Texture>(textureName);

    if(!texture) {
        ERROR_ONCE("[RENDERER]: failed to acquire texture - " + textureName);
        return;
    }

    queueRenderCall(strata, [&, texture, sRect, dRect, angleInDegrees, pivot]() {
        SDL_FRect srcRect = {sRect.x, sRect.y, sRect.w, sRect.h};
        SDL_FRect dstRect = {dRect.x, dRect.y, dRect.w, dRect.h};
        SDL_FPoint rotationPoint = {pivot.x, pivot.y};
        SDL_RenderTextureRotated(
            renderer_, texture->get(), &srcRect, &dstRect, angleInDegrees, &rotationPoint,
            SDL_FlipMode::SDL_FLIP_NONE);
    });
}

void Renderer::queueRenderTextureRotated(
    Strata strata, const std::string& textureName, Vec2 position, f32 angleInDegrees, f32 scale,
    f32 alpha) {
    auto texture = AssetManager::get()->load<Texture>(textureName);
    if(!texture) {
        ERROR_ONCE("[RENDERER]: failed to acquire texture - " + textureName);
        return;
    }

    queueRenderCall(strata, [&, texture, position, angleInDegrees, scale, alpha]() {
        f32 width, height;
        SDL_GetTextureSize(texture->get(), &width, &height);
        width *= scale;
        height *= scale;
        SDL_FRect dstRect = {position.x - width / 2.0f, position.y - height / 2.0f, width, height};
        u8 textureAlpha = static_cast<u8>(255 * alpha);
        SDL_SetTextureAlphaMod(texture->get(), textureAlpha);
        SDL_RenderTextureRotated(
            renderer_, texture->get(), nullptr, &dstRect, angleInDegrees, nullptr,
            SDL_FlipMode::SDL_FLIP_NONE);
    });
}

void Renderer::queueRenderRect(Strata strata, const Rect& rect, u8 r, u8 g, u8 b, u8 a) {
    SDL_Color color = {r, g, b, a};
    queueRenderCall(strata, [&, rect, color]() {
        SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
        SDL_FRect dRect = {rect.x, rect.y, rect.w, rect.h};
        SDL_RenderRect(renderer_, &dRect);
    });
}

void Renderer::queueRenderFilledRect(Strata strata, const Rect& rect, u8 r, u8 g, u8 b, u8 a) {
    SDL_Color color = {r, g, b, a};
    queueRenderCall(strata, [&, rect, color]() {
        SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
        SDL_FRect dRect = {rect.x, rect.y, rect.w, rect.h};
        SDL_RenderFillRect(renderer_, &dRect);
    });
}

void Renderer::queueRenderLine(Strata strata, const Line& line, u8 r, u8 g, u8 b, u8 a) {
    SDL_Color color = {r, g, b, a};
    queueRenderCall(strata, [&, line, color]() {
        SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
        SDL_RenderLine(renderer_, line.p1.x, line.p1.y, line.p2.x, line.p2.y);
    });
}

void Renderer::queueRenderCall(Strata strata, std::function<void()> renderCall) {
    switch(strata) {
        case Strata::TERRAIN:
            terrain_.push_back(renderCall);
            break;
        case Strata::ENTITY:
            entities_.push_back(renderCall);
            break;
        case Strata::EFFECT:
            effects_.push_back(renderCall);
            break;
        case Strata::UI:
            ui_.push_back(renderCall);
            break;
#ifdef DEBUG
        case Strata::DEB:
            debug_.push_back(renderCall);
            break;
#endif
        default:
            break;
    }
}

void Renderer::executeRenderCalls() {
    renderGroup(terrain_);
    renderGroup(entities_);
    renderGroup(effects_);
    renderGroup(ui_);
#ifdef DEBUG
    renderGroup(debug_);
#endif
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}

void Renderer::renderGroup(const std::vector<std::function<void()>>& group) {
    for(auto& fn : group) {
        fn();
    }
}

void Renderer::clear() {
    terrain_.clear();
    entities_.clear();
    effects_.clear();
    ui_.clear();
#ifdef DEBUG
    debug_.clear();
#endif

    SDL_SetRenderDrawColor(renderer_, 0, 50, 0, 255);
    SDL_RenderClear(renderer_);
}
