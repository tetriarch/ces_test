#include "owner.hpp"

EntityPtr OwnerComponent::owner() const {

    if(owner_.expired()) {
        return nullptr;
    }
    return owner_.lock();
}

bool OwnerComponent::isOwnedBy(EntityPtr owner) const {

    if(owner_.expired()) {
        return false;
    }

    return owner_.lock() == owner;
}

void OwnerComponent::setOwner(EntityPtr owner) {
    owner_ = owner;
}
