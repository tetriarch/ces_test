#include "geometry.hpp"
#include "../asset_manager.hpp"
#include "../entity.hpp"

void GeometryComponent::attach() {
    auto am = AssetManager::get();
    texture_ = am->load<Texture>(textureFilePath_);
}

void GeometryComponent::setTextureFilePath(const std::string& filePath) {

    textureFilePath_ = filePath;
}

void GeometryComponent::setRect(const Rect& rect) {
    rect_ = rect;
}

void GeometryComponent::render(SDL_Renderer* renderer) {

    if(!texture_) {
        ERROR_ONCE("[TEXTURE COMPONENT]: texture is empty");
        return;
    }

    Transform transform = entity()->transform();
    SDL_FRect rect;
    rect.x = transform.position.x + rect_.x;
    rect.y = transform.position.y + rect_.y;
    rect.w = rect_.w;
    rect.h = rect_.h;


    if(rect_.w != rect_.h) {
        SDL_FPoint rotationPoint;
        rotationPoint = (rect_.w > rect_.h) ? SDL_FPoint(0, rect.h / 2.0f) : SDL_FPoint(rect.w / 2, 0);

        SDL_RenderTextureRotated(renderer,
            texture_->get(),
            nullptr,
            &rect,
            transform.rotationInDegrees,
            rect.w != rect.h ? &rotationPoint : nullptr,
            SDL_FlipMode::SDL_FLIP_NONE);
    }
    else {
        SDL_RenderTextureRotated(renderer,
            texture_->get(), nullptr, &rect, transform.rotationInDegrees, nullptr, SDL_FlipMode::SDL_FLIP_NONE);
    }
}
