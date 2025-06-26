#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../math.hpp"

enum class Shape { CIRCLE, LINE, RECT };

enum class CollisionSizeDeterminant { TARGET, NONE };

struct CollisionShape : std::variant<Circle, Line, Rect> {
    using std::variant<Circle, Line, Rect>::variant;

    auto shape() -> const Shape {
        return static_cast<Shape>(index());
    }
};

struct CollisionData {
    CollisionShape shape;
    CollisionSizeDeterminant sizeDeterminant;
};

class CollisionComponent : public Component<CollisionComponent> {
public:
    void attach() override;
    void setCollisionData(const CollisionData& data);

    CollisionShape shape() const;
    bool collided() const;
    Vec2 collisionNormal() const;
    f32 collisionDepth() const;
    const std::unordered_set<EntityPtr>& colliders() const;
    void postUpdate(f32 dt) override;

private:
    bool checkCollision(EntityPtr target);
    void reposition();
    bool intersects(const CollisionShape& l, const CollisionShape& r);
    bool intersects(const Rect& l, const Rect& r);
    bool intersects(const Rect& l, const Circle& r);
    bool intersects(const Rect& l, const Line& r);
    bool intersects(const Circle& l, const Rect& r);
    bool intersects(const Circle& l, const Circle& r);
    bool intersects(const Circle& l, const Line& r);
    bool intersects(const Line& l, const Rect& r);
    bool intersects(const Line& l, const Circle& r);
    bool intersects(const Line& l, const Line& r);

    std::unordered_set<EntityPtr> colliders_;
    Vec2 collisionNormal_{0.0f, 0.0f};
    f32 collisionDepth_{0.0f};

    // actuall collision object
    CollisionShape shape_;

    // collision data by which collision object updates
    CollisionData collisionData_;
};
