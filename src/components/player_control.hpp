#pragma once

#include "../component.hpp"
#include "../entity.hpp"

class PlayerControlComponent : public Component<PlayerControlComponent> {
public:
    auto describe() -> std::string override;
    void handleInput();
};