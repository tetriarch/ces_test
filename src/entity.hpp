#pragma once

#include "component.hpp"
#include "math.hpp"

class Entity {

public:
    Entity(const std::string& name) { name_ = name; }
    static EntityPtr create(const std::string& name) { return std::make_shared<Entity>(name); }
    void addChild(EntityPtr child) { children_.emplace_back(child); child->parent_ = this; }
    void addComponent(ComponentPtr component) { components_.emplace_back(component); component->parent_ = this; }
    void setTransform(const Transform& transform) { transform_ = transform; }

    auto getChildren() const -> std::span<EntityPtr const> { return children_; }
    auto getComponents() const -> std::span<ComponentPtr const> { return components_; }
    auto getParent() const -> const Entity* { return parent_; }
    const Transform& getTransform() const { return transform_; }
    const std::string& getName() const { return name_; }

private:
    std::string name_;
    Transform transform_;
    Entity* parent_{nullptr};
    std::vector<ComponentPtr> components_;
    std::vector<EntityPtr> children_;
};