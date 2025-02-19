#pragma once

#include "../component.hpp"

class AbilityComponent : public Component<AbilityComponent> {
public:
    auto describe() -> std::string override { return "I use special attacks"; }
};