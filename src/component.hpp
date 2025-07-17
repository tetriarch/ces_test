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
    virtual void detach() {}

    virtual void handleEvents(const SDL_Event& event) {}
    virtual void update(const f32 dt) {}
    virtual void postUpdate(const f32 dt) {}
    virtual void render(std::shared_ptr<Renderer> renderer) {}
    virtual int32_t updatePriority() { return 0; }
private:
    friend struct EntityStructureModifier;

    EntityHandle entity_;
    template <class T>
    friend class Component;

    friend class Entity;

    ComponentBase() = default;
};

template <class TDerived>
class Component : public ComponentBase {
public:
    Component() {};
    auto componentType() const -> std::type_index override {
        return typeid(TDerived);
    }
};