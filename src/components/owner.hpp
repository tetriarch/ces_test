#pragma once

#include "../component.hpp"
#include "../utils.hpp"

class OwnerComponent : public Component<OwnerComponent> {

public:
    EntityPtr owner() const;
    bool isOwnedBy(EntityPtr owner) const;
    void setOwner(EntityPtr owner);

private:
    EntityHandle owner_;
};