#pragma once

#include "../component.hpp"
#include "../i_asset.hpp"
#include "../utils.hpp"

struct AnimationData : public IAsset {
    std::string name;
    u32 index;
    u32 frameCount;
    f32 frameDuration;
    bool looping;
};

class AnimationComponent : public Component<AnimationComponent> {

public:
    AnimationComponent();
    void attach() override;
    void addAnimationFiles(const std::unordered_map<std::string, std::string>& animationFiles);
    void queueAnimation(const std::string& name);
    void playAnimation(const std::string& name);
    void playAnimation(const std::string& name, std::function<void()> onAnimationEnd);
    void update(const f32 dt) override;
    void postUpdate(const f32 dt) override;
    u32 frame() const;
    u32 index() const;

private:
    void reset();

private:
    std::weak_ptr<AnimationData> currentAnimation_;
    u32 currentFrame_;
    f32 currentFrameDuration_;
    u32 index_;
    bool finished_;
    std::string queuedAnimation_;
    std::function<void()> onAnimationEnd_;
    std::unordered_map<std::string, std::string> animationFiles_;
    std::unordered_map<std::string, std::shared_ptr<AnimationData>> animations_;
};