#pragma once

#include "component.hpp"
#include "math.hpp"

class Entity : public std::enable_shared_from_this<Entity> {

public:
    Entity(const std::string& name);
    static EntityPtr create(const std::string& name);
    void addChild(EntityPtr child);
    void removeChild(const EntityPtr& child);

    void addComponent(ComponentPtr component);
    void setTransform(const Transform& transform);
    void setName(const std::string& name);

    auto children() const->std::span<EntityPtr const>;

    template<typename T>
    auto component() -> std::shared_ptr<T>;

    auto components() const->std::span<ComponentPtr const>;
    auto parent() const->Entity*;
    auto root() const -> const Entity*;
    const Transform& transform() const;
    constexpr auto  name() -> std::string const& { return name_; }

    void executeAttached();
    void handleEvents(const SDL_Event& event);
    void update();
    void render(SDL_Renderer* renderer);

private:
    std::string name_;
    Transform transform_;
    Entity* parent_{nullptr};
    std::vector<ComponentPtr> components_;
    std::vector<EntityPtr> children_;
    std::unordered_set<ComponentBase*> controllable_;
    std::unordered_set<ComponentBase*> updatable_;
    std::unordered_set<ComponentBase*> renderable_;
};

template<typename T>
inline auto Entity::component() -> std::shared_ptr<T> {

    static_assert(std::is_base_of<ComponentBase, T>::value, "T must be derived from ComponentBase");

    for(const auto& c : components_) {
        if(auto casted = std::dynamic_pointer_cast<T>(c)) {
            return casted;
        }
    }
    std::cerr << "[ENTITY]: required component not found" << std::endl;
    return nullptr;
}
