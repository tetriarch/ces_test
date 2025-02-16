#pragma once

#include "../component.hpp"

class Ability : public Component<Ability> {
public:
    auto describe() -> std::string override { return "I use special attacks"; }
};