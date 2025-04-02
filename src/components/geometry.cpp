#include "components.hpp"
#include "../asset_manager.hpp"
#include "../entity.hpp"
#include "geometry.hpp"

void GeometryComponent::attach() {
    auto am = AssetManager::get();
    texture_ = am->load<Texture>(textureFilePath_);
}

void GeometryComponent::setTextureFilePath(const std::string& filePath) {

    textureFilePath_ = filePath;
}

void GeometryComponent::setGeometryData(const GeometryData& geometryData) {

    geometryData_ = geometryData;
    rect_ = geometryData_.rect;
}


#ifdef DEBUG
void GeometryComponent::handleEvents(const SDL_Event& event) {

    if(event.type == SDL_EVENT_KEY_DOWN) {
        if(event.key.key == SDLK_F10 && event.key.mod & SDL_KMOD_LCTRL) {
            showCollisions_ = !showCollisions_;
        }
    }
}
#endif

void GeometryComponent::postUpdate(f32 dt) {

    Transform transform = entity()->transform();
    rect_.x = transform.position.x + geometryData_.rect.x;
    rect_.y = transform.position.y + geometryData_.rect.y;
}

void GeometryComponent::render(SDL_Renderer* renderer) {

    if(!texture_) {
        ERROR_ONCE("[TEXTURE COMPONENT]: texture is empty");
        return;
    }

    Transform transform = entity()->transform();
    SDL_FRect rect = {rect_.x, rect_.y, rect_.w, rect_.h};

    if(rect_.w != rect_.h) {
        SDL_FPoint rotationPoint;
        rotationPoint = (rect_.w > rect_.h) ? SDL_FPoint(0, rect.h / 2.0f) : SDL_FPoint(rect.w / 2.0f, 0);

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
            texture_->get(),
            nullptr,
            &rect,
            transform.rotationInDegrees,
            nullptr,
            SDL_FlipMode::SDL_FLIP_NONE);
    }

#ifdef DEBUG
    if(showCollisions_) {
        auto collisionComponent = entity()->component<CollisionComponent>();
        if(collisionComponent) {

            if(collisionComponent->collided()) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }

            auto shape = collisionComponent->shape();
            if(shape.shape() == Shape::RECT) {
                Rect tempRect = std::get<Rect>(shape);
                SDL_FRect rect = {tempRect.x, tempRect.y, tempRect.w, tempRect.h};
                SDL_RenderRect(renderer, &rect);
            }

            if(shape.shape() == Shape::LINE) {
                Line line = std::get<Line>(shape);
                SDL_RenderLine(renderer, line.p1.x, line.p1.y, line.p2.x, line.p2.y);
            }
        }
    }
#endif
}
