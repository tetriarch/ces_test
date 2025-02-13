#pragma once

#include "../component.hpp"

class UseAbilityComponent : public Component<UseAbilityComponent> {
public:
    auto describe() -> std::string override { return "I use special attacks"; }
};