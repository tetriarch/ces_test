#pragma once

#include "utils.hpp"
#include <SDL3/SDL.h>

class Renderer;
class ComponentBase {

public:
    virtual ~ComponentBase() = default;
    auto entity() const -> EntityPtr { return entity_.lock(); }
    virtual auto componentType() const -> std::type_index = 0;
    virtual void attach() {}
    virtual void handleEvents(const SDL_Event& event) {}
    virtual void update(const f32 dt) {}
    virtual void postUpdate(const f32 dt) {}
    virtual void render(std::shared_ptr<Renderer> renderer) {}

private:
    EntityHandle entity_;
    template <class T>
    friend class Component;

    friend class Entity;

    virtual bool hasHandleEvents() const { return true; }
    virtual bool hasUpdate() const { return true; }
    virtual bool hasPostUpdate() const { return true; }
    virtual bool hasRender() const { return true; }
    ComponentBase() = default;
};

template <class TDerived>
class Component : public ComponentBase {
public:
    Component() {};
    auto componentType() const -> std::type_index override {
        return typeid(TDerived);
    }

private:
    bool hasHandleEvents() const final {
        return !std::is_same_v<decltype(&TDerived::handleEvents), decltype(&ComponentBase::handleEvents)>;
    }

    bool hasUpdate() const final {
        return !std::is_same_v<decltype(&TDerived::update), decltype(&ComponentBase::update)>;
    }

    bool hasPostUpdate() const final {
        return !std::is_same_v<decltype(&TDerived::postUpdate), decltype(&ComponentBase::postUpdate)>;
    }

    bool hasRender() const final {
        return !std::is_same_v<decltype(&TDerived::render), decltype(&ComponentBase::render)>;
    }
};