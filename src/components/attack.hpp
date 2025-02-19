#pragma once

#include "../component.hpp"


class AttackComponent : public Component<AttackComponent> {
public:
    auto describe() -> std::string override { return "I attack"; }
};