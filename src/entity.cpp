#include "entity.hpp"

#include "component.hpp"
#include "entity_structure_modifier.hpp"
#include "scoped.hpp"

Entity::Entity(const std::string& name, bool lazyAttach)
    : name_(name), lazyAttach_(lazyAttach), active_(true) {
}

Entity::~Entity() {
    for(auto && comp : components_) {
        comp->detach();
    }
}

EntityPtr Entity::create(const std::string& name, bool lazyAttach) {
    auto e = std::make_shared<Entity>(name, lazyAttach);
    return e;
}

void Entity::addChild(EntityPtr child) {
    EntityStructureModifier::addChild(shared_from_this(), child);
}

void Entity::removeChild(const EntityPtr& child) {
    EntityStructureModifier::removeChild(shared_from_this(), child);
}

void Entity::addComponent(ComponentPtr component) {
    EntityStructureModifier::addComponent(shared_from_this(), component);
}

void Entity::removeComponent(ComponentPtr component) {
    EntityStructureModifier::removeComponent(shared_from_this(), component);
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

auto Entity::parent() const -> EntityPtr {
    return parent_.lock();
}

auto Entity::root() -> EntityPtr {
    auto current = shared_from_this();
    while(current->parent()) {
        current = current->parent();
    }
    return current;
}

const Transform& Entity::transform() const {
    return transform_;
}

void Entity::executeAttached() {
    lazyAttach_ = false;

    for(auto& c : components_) {
        c->attach();
    }

    for(auto& c : children_) {
        c->executeAttached();
    }
}

bool Entity::isActive() const {
    return active_;
}

void Entity::setActive(bool active) {
    active_ = active;
}

void Entity::handleEvents(const SDL_Event& event) {
    if (!active_) return;

    for(auto&& c : components_) {
        c->handleEvents(event);
    }

    for(auto&& c : children_) {
        c->handleEvents(event);
    }
}

void Entity::update(const f32 dt) {
    if(!active_) return;

    for(auto& u : components_) {
        u->update(dt);
    }

    for(auto& c : children_) {
        c->update(dt);
    }
}

void Entity::postUpdate(const f32 dt) {
    if (!active_) return;

    for(auto&& component : components_) {
        component->postUpdate(dt);
    }

    for(auto&& child : children_) {
        child->postUpdate(dt);
    }
}

void Entity::render(std::shared_ptr<Renderer> renderer) {
    if (!active_) return;

    for(auto& r : components_) {
        r->render(renderer);
    }

    for(auto&& c : children_) {
        c->render(renderer);
    }
}
