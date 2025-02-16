#pragma once

#include "../component.hpp"


class Attack : public Component<Attack> {
public:
    auto describe() -> std::string override { return "I attack"; }
};