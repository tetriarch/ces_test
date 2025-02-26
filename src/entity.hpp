#pragma once

#include "component.hpp"
#include "math.hpp"

class Entity {

public:
    static u32 nextID;
    Entity(const std::string& name);
    static EntityPtr create(const std::string& name);
    void addChild(EntityPtr child);
    void addComponent(ComponentPtr component);
    void setTransform(const Transform& transform);

    u32 getID();
    auto getChildren() const->std::span<EntityPtr const>;

    template<typename T>
    auto getComponent() -> std::shared_ptr<T>;

    auto getComponents() const->std::span<ComponentPtr const>;
    auto getParent() const -> const Entity*;
    const Transform& getTransform() const;
    const std::string& getName() const;

private:
    u32 ID_;
    std::string name_;
    Transform transform_;
    Entity* parent_{nullptr};
    std::vector<ComponentPtr> components_;
    std::vector<EntityPtr> children_;
};

template<typename T>
inline auto Entity::getComponent() -> std::shared_ptr<T> {

    static_assert(std::is_base_of<ComponentBase, T>::value, "T must be derived from ComponentBase");

    for(const auto& c : components_) {
        if(auto casted = std::dynamic_pointer_cast<T>(c)) {
            return casted;
        }
    }
    std::cerr << "[ENTITY]: required component not found" << std::endl;
    return nullptr;
}
