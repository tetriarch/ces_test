#pragma once

#include <SDL3/SDL.h>

#include "utils.hpp"

class Renderer;
class ComponentBase {
public:
    virtual ~ComponentBase() = default;
    auto entity() const -> EntityPtr {
        return entity_.lock();
    }
    virtual auto componentType() const -> std::type_index = 0;

    virtual void attach() {
    }
    virtual void detach() {
    }

    virtual void handleEvents(const SDL_Event& event) {
    }
    virtual void update(const f32 dt) {
    }
    virtual void postUpdate(const f32 dt) {
    }
    virtual void render(std::shared_ptr<Renderer> renderer) {
    }
    virtual s32 updatePriority() {
        return 0;
    }

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

/// A tracked component keeps track of instances of its self. These instances can be retrieved using
/// the trackedComponents function.
template<class TDerived>
class TrackedComponent : public Component<TDerived>, public std::enable_shared_from_this<TDerived> {
public:
    using Handle = std::weak_ptr<TDerived>;

    void attach() final {
        assert(trackedIndex_ == SIZE_MAX);
        s_components.push_back(this->shared_from_this());
        trackedIndex_ = s_components.size() - 1;

        onAttach();
    }

    void detach() final {
        assert(trackedIndex_ != SIZE_MAX);
        std::swap(s_components[trackedIndex_], s_components.back());
        s_components[trackedIndex_].lock()->trackedIndex_ = trackedIndex_;
        s_components.pop_back();
        trackedIndex_ = SIZE_MAX;

        onDetach();
    }

    static std::span<std::weak_ptr<TDerived> const> trackedComponents() { return s_components; }
protected:
    virtual void onAttach() {}
    virtual void onDetach() {}

private:
    size_t trackedIndex_ { SIZE_MAX };
    static inline std::vector<std::weak_ptr<TDerived>> s_components = {};
};
