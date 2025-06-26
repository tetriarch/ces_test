#pragma once

#include "../component.hpp"
#include "../utils.hpp"

class OwnerComponent : public Component<OwnerComponent> {
public:
    EntityHandle owner() const;
    bool isOwnedBy(EntityPtr owner) const;
    void setOwner(EntityPtr owner);

private:
    EntityHandle owner_;
};
