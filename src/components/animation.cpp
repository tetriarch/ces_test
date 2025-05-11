#include "animation.hpp"
#include "../asset_manager.hpp"
#include "../entity.hpp"

AnimationComponent::AnimationComponent() :
    currentFrame_(0),
    currentFrameDuration_(0.0f),
    index_(0),
    finished_(false),
    queuedAnimation_("") {

    // ctor
}

void AnimationComponent::attach() {
    for(auto& [name, file] : animationFiles_) {
        auto animation = AssetManager::get()->load<AnimationData>(file);
        if(animation) {
            animations_.insert(std::pair<std::string, std::shared_ptr<AnimationData>>(name, animation));
        }
    }

    if(!queuedAnimation_.empty()) {
        playAnimation(queuedAnimation_);
        queuedAnimation_ = "";
    }
}

void AnimationComponent::addAnimationFiles(const std::unordered_map<std::string, std::string>& animationFiles) {

    animationFiles_ = animationFiles;
}

void AnimationComponent::queueAnimation(const std::string& name) {

    queuedAnimation_ = name;
}

void AnimationComponent::playAnimation(const std::string& name) {

    if(auto it = animations_.find(name); it != animations_.end()) {
        reset();
        currentAnimation_ = it->second;
        index_ = it->second->index;
    }
    else {
        ERROR("[ANIMATION COMPONENT]: failed to play animation " + name + " for " + entity()->name());
    }
}

void AnimationComponent::playAnimation(const std::string& name, std::function<void()> onAnimationEnd) {

    if(auto it = animations_.find(name); it != animations_.end()) {
        reset();
        currentAnimation_ = it->second;
        index_ = it->second->index;
        onAnimationEnd_ = onAnimationEnd;
    }
    else {
        ERROR("[ANIMATION COMPONENT]: failed to play animation " + name + " for " + entity()->name());
    }
}

void AnimationComponent::update(const f32 dt) {

    auto animation = currentAnimation_.lock();
    if(!animation) {
        return;
    }

    if(animation->frameDuration <= 0.0f) {
        ERROR_ONCE("[ANIMATION COMPONENT]: invalid frame duration in animation: " + animation->name);
    }

    if(currentFrameDuration_ >= animation->frameDuration) {
        currentFrameDuration_ -= animation->frameDuration;

        if(currentFrame_ + 1 < animation->frameCount) {
            currentFrame_++;

        }
        else {
            if(animation->looping) {
                currentFrame_ = 0;
            }
            else {
                finished_ = true;
            }
        }
    }
    currentFrameDuration_ += dt;
}

void AnimationComponent::postUpdate(const f32 dt) {

    if(finished_ && onAnimationEnd_) {
        onAnimationEnd_();
    }
}

u32 AnimationComponent::frame() const {

    return currentFrame_;
}

u32 AnimationComponent::index() const {

    return index_;
}

void AnimationComponent::reset() {

    currentFrame_ = 0;
    currentFrameDuration_ = 0.0f;
    finished_ = false;
    onAnimationEnd_ = nullptr;
}