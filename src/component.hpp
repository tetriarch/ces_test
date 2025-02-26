#pragma once

#include "utils.hpp"

class ComponentBase {

public:
    auto getParent() const -> EntityPtr { return parent_.lock(); }
    virtual auto getComponentType() const->std::type_index = 0;
    virtual auto describe() -> std::string { return ""; }
    virtual void update() {}
    virtual void render() {}

private:
    EntityHandle parent_;
    template <class T>
    friend class Component;

    friend class Entity;

    virtual bool hasUpdate() const { return true; }
    virtual bool hasRender() const { return true; }
};

template <class TDerived>
class Component : public ComponentBase {
public:
    Component() {};
    auto getComponentType() const -> std::type_index override {
        return typeid(TDerived);
    }

private:
    bool hasUpdate() const final {
        return !std::is_same_v<decltype(&TDerived::update), decltype(&ComponentBase::update)>;
    }

    bool hasRender() const final {
        return !std::is_same_v<decltype(&TDerived::render), decltype(&ComponentBase::render)>;
    }
};