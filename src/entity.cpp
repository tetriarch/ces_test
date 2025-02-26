#include "entity.hpp"
#include "entity_manager.hpp"

//NOTE: GUID would be better for this
u32 Entity::nextID = 0;

Entity::Entity(const std::string& name)
    : ID_(nextID++),
    name_(name) {

}


EntityPtr Entity::create(const std::string& name) {
    auto e = std::make_shared<Entity>(name);
    EntityManager::get().addEntity(e);
    return e;
}



void Entity::addChild(EntityPtr child) {
    children_.emplace_back(child); child->parent_ = this;
}



void Entity::addComponent(ComponentPtr component) {
    components_.emplace_back(component); component->parent_ = this;
}



void Entity::setTransform(const Transform& transform) {
    transform_ = transform;
}



u32 Entity::getID() {
    return ID_;
}



auto Entity::getChildren() const -> std::span<EntityPtr const> {
    return children_;
}



auto Entity::getComponents() const -> std::span<ComponentPtr const> {
    return components_;
}



auto Entity::getParent() const -> const Entity* {
    return parent_;
}



const Transform& Entity::getTransform() const {
    return transform_;
}



const std::string& Entity::getName() const {
    return name_;
}

