#include "owner.hpp"

EntityHandle OwnerComponent::owner() const {
    return owner_;
}

bool OwnerComponent::isOwnedBy(EntityPtr owner) const {
    return owner_.lock() == owner;
}

void OwnerComponent::setOwner(EntityPtr owner) {
    owner_ = owner;
}
