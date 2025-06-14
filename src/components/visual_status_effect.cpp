#include "visual_status_effect.hpp"

#include "../entity.hpp"
#include "../renderer.hpp"
#include "animation.hpp"

void VisualStatusEffectComponent::setTextureFile(const std::string& textureFilePath) {
    textureFilePath_ = textureFilePath;
}

void VisualStatusEffectComponent::setRect(const Rect& rect) {
    rect_ = rect;
}

void VisualStatusEffectComponent::render(std::shared_ptr<Renderer> renderer) {
    // get transform of the parent to which we display the status effect
    auto transform = entity()->parent()->transform();
    Rect sRect = rect_;
    sRect.x = 0;
    sRect.y = 0;

    Rect dRect = rect_;
    dRect.x += transform.position.x;
    dRect.y += transform.position.y;

    // if animated
    auto animationComponent = entity()->component<AnimationComponent>();
    if(animationComponent) {
        sRect.x = animationComponent->frame() * sRect.w;
        sRect.y = animationComponent->index() * sRect.h;

        renderer->queueRenderTexture(Strata::EFFECT, textureFilePath_, sRect, dRect);
        return;
    };

    renderer->queueRenderTexture(Strata::EFFECT, textureFilePath_, sRect, dRect);
}
