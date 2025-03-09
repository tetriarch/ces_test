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

    assert(component->entity_.expired());

    component->entity_ = shared_from_this();
    components_.push_back(component);

    if(component->hasHandleEvents()) {
        controllable_.insert(component.get());
    }

    if(component->hasUpdate()) {
        updatable_.insert(component.get());
    }

    if(component->hasRender()) {
        renderable_.insert(component.get());
    }
}

void Entity::setTransform(const Transform& transform) {
    transform_ = transform;
}

void Entity::setName(const std::string& name) {
    name_ = name;
}

auto Entity::children() const -> std::span<EntityPtr const> {
    return children_;
}

auto Entity::components() const -> std::span<ComponentPtr const> {
    return components_;
}

auto Entity::parent() const -> Entity* {
    return parent_;
}

auto Entity::root() const -> const Entity* {

    auto current = this;
    while(current->parent_) {
        current = current->parent_;
    }
    return current;
}

const Transform& Entity::transform() const {
    return transform_;
}

void Entity::executeAttached() {

    for(auto& c : components_) {
        c->attach();
    }

    for(auto& c : children_) {
        c->executeAttached();
    }
}

void Entity::handleEvents(const SDL_Event& event) {

    for(auto& c : controllable_) {
        c->handleEvents(event);
    }

    for(auto& c : children_) {
        c->handleEvents(event);
    }
}

void Entity::update() {

    for(auto& u : updatable_) {
        u->update();
    }

    for(auto& c : children_) {
        c->update();
    }
}

void Entity::render(SDL_Renderer* renderer) {

    for(auto& r : renderable_) {
        r->render(renderer);
    }

    for(auto& c : children_) {
        c->render(renderer);
    }
}
