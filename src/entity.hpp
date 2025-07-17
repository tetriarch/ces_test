#pragma once

#include <SDL3/SDL.h>

#include "log.hpp"
#include "math.hpp"

class Renderer;

class Entity : public std::enable_shared_from_this<Entity> {
public:
    Entity(const std::string& name, bool lazyAttach);

    static EntityPtr create(const std::string& name, bool lazyAttach = false);
    void addChild(EntityPtr child);
    void removeChild(const EntityPtr& child);

    void addComponent(ComponentPtr component);
    void removeComponent(ComponentPtr component);
    void setTransform(const Transform& transform);
    void setName(const std::string& name);

    auto children() const -> std::span<EntityPtr const>;

    template <typename T>
    auto component() -> std::shared_ptr<T>;

    auto components() const -> std::span<ComponentPtr const>;

    auto parent() const -> EntityPtr;
    auto root() -> EntityPtr;

    const Transform& transform() const;
    u32 id() {
        return ID_;
    }
    constexpr auto name() -> std::string const& {
        return name_;
    }

    void executeAttached();
    bool isActive() const;
    void setActive(bool active = true);
    void handleEvents(const SDL_Event& event);
    void update(const f32 dt);
    void postUpdate(const f32 dt);
    void render(std::shared_ptr<Renderer> renderer);

private:
    static u32 NEXT_ID;

    enum class UpdateState { IDLE, UPDATE };

private:
    friend struct EntityStructureModifier;

    u32 ID_;
    std::string name_;
    Transform transform_;
    EntityHandle parent_;
    std::vector<ComponentPtr> components_;
    std::vector<EntityPtr> children_;
    bool lazyAttach_;
    bool active_;
};

template <typename T>
inline auto Entity::component() -> std::shared_ptr<T> {
    static_assert(std::is_base_of<ComponentBase, T>::value, "T must be derived from ComponentBase");

    for(const auto& c : components_) {
        if(auto casted = std::dynamic_pointer_cast<T>(c)) {
            return casted;
        }
    }
    return nullptr;
}
