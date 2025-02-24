#pragma once

#include "../component.hpp"

struct Life {
    u32 current{0};
    u32 max{0};
};

class LifeComponent : public Component<LifeComponent> {

public:
    const Life& getLife() const;
    void setLife(const Life& life);
    void update();
    auto describe() -> std::string override;

private:
    Life life_;
};