#pragma once

#include "../component.hpp"
#include "../entity.hpp"

class PlayerControlComponent : public Component<PlayerControlComponent> {

public:
    Vec2 getMousePosition();
    void handleEvents(const SDL_Event& event) override;
    void update(f32 dt) override;
private:
    Vec2 mousePosition_;
};