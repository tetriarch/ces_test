#include "entity.hpp"
#include "entity_manager.hpp"


Entity::Entity(const std::string& name)
    : name_(name) {

}



EntityPtr Entity::create(const std::string& name) {
    auto e = std::make_shared<Entity>(name);
    EntityManager::get().addEntity(e);
    return e;
}



void Entity::addChild(EntityPtr child) {
    children_.emplace_back(child); child->parent_ = this;
}

void Entity::removeChild(const EntityPtr& child) {
    assert(child->parent_ == this);
    child->parent_ = nullptr;
    children_.erase(std::remove(children_.begin(), children_.end(), child), children_.end());
}



void Entity::addComponent(ComponentPtr component) {
    assert(component->parent_.expired());
    component->parent_ = shared_from_this();
    components_.push_back(component);
}



void Entity::setTransform(const Transform& transform) {
    transform_ = transform;
}




auto Entity::getChildren() const -> std::span<EntityPtr const> {
    return children_;
}



auto Entity::getComponents() const -> std::span<ComponentPtr const> {
    return components_;
}



auto Entity::getParent() const -> Entity* {
    return parent_;
}



auto Entity::getRoot() const -> const Entity* {

    auto current = this;
    while(current->parent_) {
        current = current->parent_;
    }
    return current;
}



const Transform& Entity::getTransform() const {
    return transform_;
}



const std::string& Entity::getName() const {
    return name_;
}

