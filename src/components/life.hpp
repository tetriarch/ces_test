#pragma once

#include "../component.hpp"

struct Life {
    u32 current{0};
    u32 max{0};
};

class LifeComponent : public Component<LifeComponent> {

public:
    const Life& life() const;
    void setLife(const Life& life);

private:
    Life life_;
};