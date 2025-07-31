#include "geometry.hpp"

#include "../entity.hpp"
#include "../renderer.hpp"
#include "animation.hpp"

void GeometryComponent::setTextureFilePath(const std::string& filePath) {
    textureFilePath_ = filePath;
}

void GeometryComponent::setGeometryData(const GeometryData& geometryData) {
    geometryData_ = geometryData;
    rect_ = geometryData_.rect;
}

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
        Strata::ENTITY, textureFilePath_, sRect, dRect, rotationPoint, transform.rotation);
}

Rect GeometryComponent::rect() const {
    return rect_;
}
