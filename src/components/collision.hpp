#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../math.hpp"
#include "../signal.h"

enum class Shape { CIRCLE, LINE, RECT };

enum class CollisionSizeDeterminant { TARGET, NONE };

struct CollisionShape : std::variant<Circle, Line, Rect> {
    using std::variant<Circle, Line, Rect>::variant;

    [[nodiscard]] auto shape() const -> Shape {
        return static_cast<Shape>(index());
    }
};

std::tuple<bool, Vec2, float> intersects(const CollisionShape& l, const CollisionShape& r);
std::tuple<bool, Vec2, float> intersects(const Rect& l, const Rect& r);
std::tuple<bool, Vec2, float> intersects(const Rect& l, const Circle& r);
std::tuple<bool, Vec2, float> intersects(const Rect& l, const Line& r);
std::tuple<bool, Vec2, float> intersects(const Circle& l, const Rect& r);
std::tuple<bool, Vec2, float> intersects(const Circle& l, const Circle& r);
std::tuple<bool, Vec2, float> intersects(const Circle& l, const Line& r);
std::tuple<bool, Vec2, float> intersects(const Line& l, const Rect& r);
std::tuple<bool, Vec2, float> intersects(const Line& l, const Circle& r);
std::tuple<bool, Vec2, float> intersects(const Line& l, const Line& r);

struct CollisionData {
    CollisionShape shape;
    CollisionSizeDeterminant sizeDeterminant;
};

class CollisionComponent;
class CollisionSystem : public Component<CollisionSystem> {
public:
    void update(f32 dt) override;
    void handleEvents(const SDL_Event& event) override;
    void render(std::shared_ptr<Renderer> renderer) override;

    size_t addOnCollisionListener(std::function<void(EntityPtr, EntityPtr)> callback) {
        return onCollision_.subscribe(std::move(callback));
    }

    void removeOnCollisionListener(size_t id) {
        onCollision_.unsubscribe(id);
    }

private:
    Signal<EntityPtr, EntityPtr> onCollision_;
    std::unordered_set<CollisionComponent*> collisions_;
    bool showCollisions_{true};
};

class CollisionComponent : public TrackedComponent<CollisionComponent> {
public:
    void setCollisionShape(const CollisionShape& shape);
    CollisionShape shape() const;
    bool collided() const;
    Vec2 collisionNormal() const;
    f32 collisionDepth() const;
    const std::vector<EntityHandle>& colliders() const;

    size_t addOnCollisionListener(std::function<void(EntityPtr, Vec2, float)> callback) {
        return onCollision_.subscribe(std::move(callback));
    }

    void removeOnCollisionListener(size_t id) {
        onCollision_.unsubscribe(id);
    }

private:
    friend class CollisionSystem;

    std::vector<EntityHandle> colliders_;
    Vec2 collisionNormal_{0.0f, 0.0f};
    f32 collisionDepth_{0.0f};

    CollisionShape shape_;
    Signal<EntityPtr, Vec2, float> onCollision_;
};
