#include "collision.hpp"
#include "../entity_manager.hpp"
#include "../entity.hpp"

void CollisionComponent::attach() {
    reposition();
}

void CollisionComponent::setCollisionData(const CollisionData& data) {
    collisionData_ = data;
    shape_ = data.shape;
}

CollisionShape CollisionComponent::shape() const {
    return shape_;
}

bool CollisionComponent::checkCollision(EntityPtr target) {

    auto targetCollisionComponent = target->component<CollisionComponent>();

    if(!targetCollisionComponent) {
        return false;
    }

    auto targetShape = targetCollisionComponent->shape();

    if(intersects(shape_, targetShape)) {
        return true;
    }

    return false;
}

bool CollisionComponent::collided() const {

    return collided_;
}

Vec2 CollisionComponent::collisionNormal() const {

    return collisionNormal_;
}

f32 CollisionComponent::collisionDepth() const {

    return collisionDepth_;
}

void CollisionComponent::postUpdate(f32 dt) {

    reposition();

    collisionNormal_ = {0.0f, 0.0f};
    collisionDepth_ = 0.0f;

    auto entitties = EntityManager::get().entities();

    for(auto&& e : entitties) {

        if(e.second.expired()) {
            continue;
        }
        auto ePtr = e.second.lock();

        // skip ourselves
        if(ePtr.get() == entity()->shared_from_this().get()) {
            continue;
        }

        collided_ = checkCollision(ePtr);
        // stop checking collisions if collided
        if(collided_) {
            return;
        }
    }
}

void CollisionComponent::reposition() {
    auto transform = entity()->transform();
    if(shape_.shape() == Shape::RECT) {
        auto rect = std::get<Rect>(shape_);
        auto dataRect = std::get<Rect>(collisionData_.shape);
        rect.x = transform.position.x + dataRect.x;
        rect.y = transform.position.y + dataRect.y;
        shape_ = rect;
    }

    if(shape_.shape() == Shape::LINE) {
        auto line = std::get<Line>(shape_);
        line.p1.x = transform.position.x;
        line.p1.y = transform.position.y;
        shape_ = line;
    }

    if(shape_.shape() == Shape::CIRCLE) {
        auto circle = std::get<Circle>(shape_);
        circle.x = transform.position.x;
        circle.y = transform.position.y;
        shape_ = circle;
    }
}

bool CollisionComponent::intersects(const CollisionShape& lsh, const CollisionShape& rsh) {

    return std::visit([&](const auto& lShape, const auto& rShape) -> bool {
        return this->intersects(lShape, rShape);
    }, lsh, rsh);
    return false;
}

bool CollisionComponent::intersects(const Rect& l, const Rect& r) {

    if(l.x < r.x + r.w &&
        l.x + l.w > r.x &&
        l.y < r.y + r.h &&
        l.y + l.h > r.y) {

        f32 overlapX = std::min(l.x + l.w, r.x + r.w) - std::max(l.x, r.x);
        f32 overlapY = std::min(l.y + l.h, r.y + r.h) - std::max(l.y, r.y);

        Vec2 normal;
        if(overlapX > overlapY) {
            normal = (l.y > r.y) ? Vec2(0.0f, -1.0f) : Vec2(0.0f, 1.0f);
        }
        else {
            normal = (l.x > r.x) ? Vec2(-1.0f, 0.0f) : Vec2(1.0f, 0.0f);
        }
        collisionNormal_ = normal;
        collisionDepth_ = std::min(overlapX, overlapY);
        return true;
    }
    return false;
}

bool CollisionComponent::intersects(const Rect& l, const Circle& r) {

    // AABB check first
    if(r.x > l.x && r.x < l.x + l.w && r.y > l.y && r.y < l.y + l.h) {
        return true;
    }

    f32 rectMinX, rectMaxX, rectMinY, rectMaxY;

    rectMinX = l.x;
    rectMaxX = l.x + l.w;
    rectMinY = l.y;
    rectMaxY = l.y + l.h;

    Line edges[4];
    // top
    edges[0].p1 = {rectMinX, rectMinY};
    edges[0].p2 = {rectMaxX, rectMinY};

    // right
    edges[1].p1 = {rectMaxX, rectMinY};
    edges[1].p2 = {rectMaxX, rectMaxY};

    // bottom
    edges[2].p1 = {rectMinX, rectMaxY};
    edges[2].p2 = {rectMaxX, rectMaxY};

    // left
    edges[3].p1 = {rectMinX, rectMinY};
    edges[3].p2 = {rectMinX, rectMaxY};

    for(auto& e : edges) {
        if(intersects(e, r)) {
            return true;
        }
    }

    return false;
}

bool CollisionComponent::intersects(const Rect& l, const Line& r) {

    f32 rectMinX, rectMaxX, rectMinY, rectMaxY;
    f32 lineMinX, lineMaxX, lineMinY, lineMaxY;

    rectMinX = l.x;
    rectMaxX = l.x + l.w;
    rectMinY = l.y;
    rectMaxY = l.y + l.h;

    lineMinX = std::min(r.p1.x, r.p2.x);
    lineMaxX = std::max(r.p1.x, r.p2.x);
    lineMinY = std::min(r.p1.y, r.p2.y);
    lineMaxY = std::max(r.p1.y, r.p2.y);

    f32 overlapX = std::min(rectMaxX, lineMaxX) - std::max(rectMinX, lineMinX);
    f32 overlapY = std::min(rectMaxY, lineMaxY) - std::max(rectMinY, lineMinY);

    // no overlap
    if(overlapX < 0 || overlapY < 0) {
        return false;
    }

    Line edges[4];
    // top
    edges[0].p1 = {rectMinX, rectMinY};
    edges[0].p2 = {rectMaxX, rectMinY};

    // right
    edges[1].p1 = {rectMaxX, rectMinY};
    edges[1].p2 = {rectMaxX, rectMaxY};

    // bottom
    edges[2].p1 = {rectMinX, rectMaxY};
    edges[2].p2 = {rectMaxX, rectMaxY};

    // left
    edges[3].p1 = {rectMinX, rectMinY};
    edges[3].p2 = {rectMinX, rectMaxY};

    // if at least one edge intersects with the line
    for(auto& e : edges) {
        if(intersects(e, r)) {
            return true;
        }
    }

    return false;
}

bool CollisionComponent::intersects(const Circle& l, const Rect& r) {
    return intersects(r, l);
}

bool CollisionComponent::intersects(const Circle& l, const Circle& r) {

    auto lrx = pow(l.x - r.x, 2);
    auto lry = pow(l.y - r.y, 2);

    // square distance between centers
    auto squareDistance = lrx + lry;

    auto squareRadius = pow(l.r + r.r, 2);

    return squareDistance <= squareRadius;
}

bool CollisionComponent::intersects(const Circle& l, const Line& r) {

    Vec2 center = {l.x, l.y};
    Vec2 lineToCenter = center - r.p1;
    Vec2 lineDirection = r.p2 - r.p1;

    f32 numerator = math::dot(lineToCenter, lineDirection);
    f32 denominator = math::dot(lineDirection, lineDirection);

    // line is a point
    if(denominator == 0) {
        return false;
    }
    f32 t = numerator / denominator;
    t = std::clamp(t, 0.0f, 1.0f);

    Vec2 p = r.p1 + t * lineDirection;

    f32 squareDistance = math::dot(center - p, center - p);
    f32 squareRadius = l.r * l.r;

    return squareDistance <= squareRadius;
}

bool CollisionComponent::intersects(const Line& l, const Rect& r) {
    return intersects(r, l);
}

bool CollisionComponent::intersects(const Line& l, const Circle& r) {
    return intersects(r, l);
}

bool CollisionComponent::intersects(const Line& l, const Line& r) {

    Vec2 LtoR = r.p1 - l.p1;
    Vec2 lDirection = l.p2 - l.p1;
    Vec2 rDirection = r.p2 - r.p1;

    // relative direction between lines
    f32 numerator = math::cross(LtoR, rDirection);

    f32 denominator = math::cross(lDirection, rDirection);

    // lines are parallel
    if(denominator == 0.0f) {
        return false;
    }

    f32 t = numerator / denominator;

    if(t >= 0.0f && t <= 1.0f) {
        return true;
    }

    return false;
}
