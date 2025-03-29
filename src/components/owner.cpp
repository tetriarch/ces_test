#include "owner.hpp"

EntityPtr OwnerComponent::owner() const {

    if(owner_.expired()) {
        return nullptr;
    }
    return owner_.lock();
}

void OwnerComponent::setOwner(EntityPtr owner) {
    owner_ = owner;
}
