#include "geometry.hpp"

#include "../entity.hpp"
#include "../renderer.hpp"
#include "animation.hpp"
#include "collision.hpp"

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
            if (auto collisionComponent = std::make_shared<CollisionComponent>()) {
                showCollisions_ = !showCollisions_;

                if (showCollisions_) {
                    onCollisionId_ = collisionComponent->addOnCollisionListener(
                        [this](auto const&, auto, auto) {
                            this->hit_ = true;
                        });
                }
                else {
                    collisionComponent->removeOnCollisionListener(onCollisionId_);
                    onCollisionId_ = SIZE_MAX;
                }
            }
        }
    }
}
#endif

void GeometryComponent::postUpdate(f32 dt) {
    Transform transform = entity()->transform();
    rect_.x = transform.position.x + geometryData_.rect.x;
    rect_.y = transform.position.y + geometryData_.rect.y;
}

void GeometryComponent::render(std::shared_ptr<Renderer> renderer) {
    Transform transform = entity()->transform();
    Rect sRect = {0.0f, 0.0f, rect_.w, rect_.h};
    Rect dRect = {rect_.x, rect_.y, rect_.w, rect_.h};

    auto animationComponent = entity()->component<AnimationComponent>();
    if(animationComponent) {
        sRect.x = animationComponent->frame() * rect_.w;
        sRect.y = animationComponent->index() * rect_.h;
    }

    Vec2 rotationPoint;
    if(rect_.w > rect_.h) {
        rotationPoint = {0.0f, dRect.h / 2.0f};
    } else if(rect_.w < rect_.h) {
        rotationPoint = {dRect.w / 2.0f, 0.0f};
    } else {
        rotationPoint = {dRect.w / 2.0f, dRect.h / 2.0f};
    }

    renderer->queueRenderTextureRotated(
        Strata::ENTITY, textureFilePath_, sRect, dRect, rotationPoint, transform.rotationInDegrees);

#ifdef DEBUG
    if(showCollisions_) {
        auto collisionComponent = entity()->component<CollisionComponent>();
        bool collided = hit_;
        hit_ = false;

        auto shape = collisionComponent->shape();
        if(shape.shape() == Shape::RECT) {
            Rect debugRect = std::get<Rect>(shape);
            if(collided) {
                renderer->queueRenderRect(Strata::DEB, debugRect, 255, 0, 0, 255);
            } else {
                renderer->queueRenderRect(Strata::DEB, debugRect);
            }
        }
        if(shape.shape() == Shape::LINE) {
            Line debugLine = std::get<Line>(shape);
            if(collided) {
                renderer->queueRenderLine(Strata::DEB, debugLine, 255, 0, 0, 255);
            } else {
                renderer->queueRenderLine(Strata::DEB, debugLine);
            }
        }
    }
#endif
}

Rect GeometryComponent::rect() const {
    return rect_;
}
