#pragma once

#include "../component.hpp"
#include "../entity.hpp"

class PlayerControlComponent : public Component<PlayerControlComponent> {

public:
    void handleEvents(const SDL_Event& event);
};