#include "component.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"
#include "entity_structure_modifier.hpp"

EntityState EntityStructureModifier::state_ = EntityState::Idle;
std::vector<std::function<void()>> EntityStructureModifier::modifications_;

void EntityStructureModifier::addChild(
        EntityPtr parent,
        EntityPtr child,
        StructureUpdateType updateType) {
    if (updateType == StructureUpdateType::Deferred && state_ == EntityState::Updating) {
        modifications_.push_back([parent, child]() {
            addChild(parent, child, StructureUpdateType::Immediate);
        });
    }

    assert(child->parent() == nullptr);
    child->parent_ = parent;
    parent->children_.push_back(child);

    if (!parent->lazyAttach_ && child->lazyAttach_) {
        child->executeAttached();
    }
}

void EntityStructureModifier::removeChild(
    EntityPtr parent,
    EntityPtr child,
    StructureUpdateType updateType) {
    if (updateType == StructureUpdateType::Deferred && state_ == EntityState::Updating) {
        modifications_.push_back([parent, child]() {
            removeChild(parent, child, StructureUpdateType::Immediate);
        });
    }

    assert(child->parent_.lock() == parent);
    child->parent_.reset();
    parent->children_.erase(
        std::remove(parent->children_.begin(), parent->children_.end(), child),
        parent->children_.end());

    EntityManager::get().removeEntity(child);
}

void EntityStructureModifier::addComponent(
    EntityPtr parent,
    ComponentPtr component,
    StructureUpdateType updateType) {
    if (updateType == StructureUpdateType::Deferred && state_ == EntityState::Updating) {
        modifications_.push_back([parent, component]() {
            addComponent(parent, component, StructureUpdateType::Immediate);
        });
    }

    assert(component->entity_.expired());
    component->entity_ = parent;
    parent->components_.push_back(component);

    if (!parent->lazyAttach_) {
        component->attach();
    }
}

void EntityStructureModifier::removeComponent(
    EntityPtr parent,
    ComponentPtr component,
    StructureUpdateType updateType) {
    if (updateType == StructureUpdateType::Deferred && state_ == EntityState::Updating) {
        modifications_.push_back([parent, component]() {
            removeComponent(parent, component, StructureUpdateType::Immediate);
        });
    }

    assert(component->entity_.lock() == parent);
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
}