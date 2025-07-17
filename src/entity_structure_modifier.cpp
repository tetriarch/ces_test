#include "entity_structure_modifier.hpp"

#include "component.hpp"
#include "entity.hpp"

EntityState EntityStructureModifier::state_ = EntityState::Idle;
std::vector<std::function<void()>> EntityStructureModifier::modifications_;

void EntityStructureModifier::addChild(
    EntityPtr parent, EntityPtr child, StructureUpdateType updateType) {
    if(updateType == StructureUpdateType::Deferred && state_ == EntityState::Updating) {
        modifications_.push_back(
            [parent, child]() { addChild(parent, child, StructureUpdateType::Immediate); });
        return;
    }

    assert(child->parent() == nullptr);
    child->parent_ = parent;
    parent->children_.push_back(child);

    if(!parent->lazyAttach_ && child->lazyAttach_) {
        child->executeAttached();
    }
}

void EntityStructureModifier::removeChild(
    EntityPtr parent, EntityPtr child, StructureUpdateType updateType) {
    if(updateType == StructureUpdateType::Deferred && state_ == EntityState::Updating) {
        modifications_.push_back(
            [parent, child]() { removeChild(parent, child, StructureUpdateType::Immediate); });
        return;
    }

    assert(child->parent_.lock() == parent);
    child->parent_.reset();
    parent->children_.erase(std::remove(parent->children_.begin(), parent->children_.end(), child),
        parent->children_.end());
}

void EntityStructureModifier::addComponent(
    EntityPtr parent, ComponentPtr component, StructureUpdateType updateType) {
    if(updateType == StructureUpdateType::Deferred && state_ == EntityState::Updating) {
        modifications_.push_back([parent, component]() {
            addComponent(parent, component, StructureUpdateType::Immediate);
        });
        return;
    }

    assert(component->entity_.expired());
    component->entity_ = parent;

    // Insertion sort, based on the component priority, this ensures that a component that should
    // update before another component always appears in the list of components _before_ the latter
    // one. Note that this has NO EFFECT on components in OTHER entities.
    parent->components_.insert(
        std::lower_bound(parent->components_.begin(), parent->components_.end(), component,
            [](auto const& lhs, auto const& rhs) {
                return lhs->updatePriority() < rhs->updatePriority();
            }),
        component);

    if(!parent->lazyAttach_) {
        component->attach();
    }
}

void EntityStructureModifier::removeComponent(
    EntityPtr parent, ComponentPtr component, StructureUpdateType updateType) {
    if(updateType == StructureUpdateType::Deferred && state_ == EntityState::Updating) {
        modifications_.push_back([parent, component]() {
            removeComponent(parent, component, StructureUpdateType::Immediate);
        });
    }

    assert(component->entity_.lock() == parent);

    component->detach();
    component->entity_.reset();
    parent->components_.erase(
        std::remove(parent->components_.begin(), parent->components_.end(), component),
        parent->components_.end());
}

void EntityStructureModifier::beginUpdate() {
    assert(state_ == EntityState::Idle);
    state_ = EntityState::Updating;
}

void EntityStructureModifier::endUpdate() {
    assert(state_ == EntityState::Updating);
    state_ = EntityState::Idle;
}

void EntityStructureModifier::applyStructureModifications() {
    assert(state_ == EntityState::Idle);
    for(auto& mod : modifications_) {
        mod();
    }
    modifications_.clear();
}
