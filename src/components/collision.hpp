#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../math.hpp"
#include "../event.h"

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

    EventF<CollisionSystem, EntityPtr, EntityPtr> onCollision;
private:
    std::unordered_set<CollisionComponent*> collisions_;
    bool showCollisions_{true};
};

class CollisionComponent : public TrackedComponent<CollisionComponent> {
public:
    void setCollisionShape(const CollisionShape& shape);
    CollisionShape shape() const;

    EventF<CollisionSystem, EntityPtr, Vec2, float> onCollision;
private:
    CollisionShape shape_;
};
