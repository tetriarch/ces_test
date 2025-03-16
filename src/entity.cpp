#include "entity.hpp"
#include "entity_manager.hpp"
#include "scoped.hpp"
#include "component.hpp"

Entity::Entity(const std::string& name, bool lazyAttach) :
    name_(name),
    lazyAttach_(lazyAttach) {

}

EntityPtr Entity::create(const std::string& name, bool lazyAttach) {
    auto e = std::make_shared<Entity>(name, lazyAttach);
    EntityManager::get().addEntity(e);
    return e;
}

void Entity::addChild(EntityPtr child) {

    if(updateState_ == UpdateState::UPDATE) {
        postUpdateActions_.push_back([child = std::move(child)](Entity* self) {
            self->addChild(std::move(child));
        });
        return;
    }

    child->parent_ = this;
    auto childPtr = child.get();
    children_.push_back(std::move(child));

    if(!lazyAttach_ && childPtr->lazyAttach_) {
        childPtr->executeAttached();
    }

}

void Entity::removeChild(const EntityPtr& child) {

    assert(child->parent_ == this);

    child->parent_ = nullptr;
    children_.erase(std::remove(children_.begin(), children_.end(), child), children_.end());
}

void Entity::addComponent(ComponentPtr component) {

    if(updateState_ == UpdateState::UPDATE) {
        postUpdateActions_.push_back(
            [component = std::move(component)](Entity* self) {
            self->addComponent(std::move(component));
        });
        return;
    }

    assert(component->entity_.expired());

    component->entity_ = shared_from_this();

    if(component->hasHandleEvents()) {
        controllable_.insert(component.get());
    }

    if(component->hasUpdate()) {
        updatable_.insert(component.get());
    }

    if(component->hasRender()) {
        renderable_.insert(component.get());
    }

    components_.push_back(component);

    if(!lazyAttach_) {
        component->attach();
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

auto Entity::root() -> Entity* {

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

    lazyAttach_ = false;

    for(auto& c : components_) {
        c->attach();
    }

    for(auto& c : children_) {
        c->executeAttached();
    }
}

void Entity::handleEvents(const SDL_Event& event) {

    {
        updateState_ = UpdateState::UPDATE;
        SCOPED([this]() { updateState_ = UpdateState::IDLE;});
        for(auto&& c : controllable_) {
            c->handleEvents(event);
        }


        for(auto&& c : children_) {
            c->handleEvents(event);
        }
    }
    applyPostUpdateActions();

}

void Entity::update(const f32 dt) {

    {
        updateState_ = UpdateState::UPDATE;
        SCOPED([this]() { updateState_ = UpdateState::IDLE;});
        for(auto& u : updatable_) {
            u->update(dt);
        }

        for(auto& c : children_) {
            c->update(dt);
        }
    }
    applyPostUpdateActions();
}

void Entity::postUpdate(const f32 dt) {

    {
        updateState_ = UpdateState::UPDATE;
        SCOPED([this]() { updateState_ = UpdateState::IDLE;});

        for(auto&& component : postUpdatable_) {
            component->postUpdate(dt);
        }

        for(auto&& child : children_) {
            child->postUpdate(dt);
        }
    }

    applyPostUpdateActions();
}

void Entity::applyPostUpdateActions() {

    if(updateState_ != UpdateState::IDLE) {
        FATAL_ERROR("[ENTITY]: update state is not idle");
    }
    for(auto&& action : postUpdateActions_) {
        action(this);
    }
    postUpdateActions_ = {};
}

void Entity::render(SDL_Renderer* renderer) {

    for(auto& r : renderable_) {
        r->render(renderer);
    }

    for(auto&& c : children_) {
        c->render(renderer);
    }
}
