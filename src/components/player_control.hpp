#pragma once

#include "../component.hpp"
#include "../entity.hpp"

class PlayerControlComponent : public Component<PlayerControlComponent> {

public:
    void handleEvents(const SDL_Event& event);
    void update(f32 dt) override;
private:
    Vec2 mousePosition_;
};