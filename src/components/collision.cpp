#include "collision.hpp"

#include "../entity.hpp"
#include "../renderer.hpp"

void CollisionSystem::update(f32 dt) {
    auto span = CollisionComponent::trackedComponents();

    collisions_.clear();
    for(size_t i = 0; i < span.size(); ++i) {
        for(size_t j = i + 1; j < span.size(); ++j) {
            auto lhs = span[i].lock();
            assert(lhs);
            auto rhs = span[j].lock();
            assert(rhs);

            auto lhsE = lhs->entity();
            assert(lhsE);

            auto rhsE = rhs->entity();
            assert(rhsE);

            if(auto [suc, normal, depth] = intersects(lhs->shape(), rhs->shape()); suc) {
                // Global collision event
                collisions_.emplace(lhs.get());
                collisions_.emplace(rhs.get());
                onCollision(lhsE, rhsE);

                // Individual collision event
                lhs->onCollision(rhsE, normal, depth);
                rhs->onCollision(lhsE, -normal, depth);
            }
        }
    }
}

void CollisionSystem::handleEvents(const SDL_Event& event) {
    if(event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F10 &&
       event.key.mod & SDL_KMOD_LCTRL) {
        showCollisions_ = !showCollisions_;
    }
}

void CollisionSystem::render(std::shared_ptr<Renderer> renderer) {
#ifdef DEBUG
    if(showCollisions_) {
        auto span = CollisionComponent::trackedComponents();
        for(auto&& weakCol : span) {
            auto collider = weakCol.lock();
            auto collided = collisions_.contains(collider.get());

            std::visit(
                overloaded{
                    [&](const Rect& debugRect) {
                        if(collided) {
                            renderer->queueRenderRect(Strata::DEB, debugRect, 255, 0, 0, 255);
                        } else {
                            renderer->queueRenderRect(Strata::DEB, debugRect);
                        }
                    },
                    [&](const Line& debugLine) {
                        if(collided) {
                            renderer->queueRenderLine(Strata::DEB, debugLine, 255, 0, 0, 255);
                        } else {
                            renderer->queueRenderLine(Strata::DEB, debugLine);
                        }
                    },
                    [&](const Circle& debugCircle) {},
                },
                collider->shape());
        }
    }
#endif
}

void CollisionComponent::setCollisionShape(const CollisionShape& shape) {
    shape_ = shape;
}

CollisionShape CollisionComponent::shape() const {
    auto transform = entity()->transform();

    return std::visit(
        overloaded{
            [&transform](Rect rect) {
                rect.x += transform.position.x;
                rect.y += transform.position.y;
                return CollisionShape{rect};
            },
            [&transform](Line line) {
                line.p1.x = transform.position.x;
                line.p1.y = transform.position.y;
                return CollisionShape{line};
            },
            [&transform](Circle circle) {
                circle.x = transform.position.x;
                circle.y = transform.position.y;
                return CollisionShape{circle};
            }},
        shape_);
}

std::tuple<bool, Vec2, float> intersects(const CollisionShape& lsh, const CollisionShape& rsh) {
    return std::visit(
        [](auto const& lhs, auto const& rhs) { return intersects(lhs, rhs); }, lsh, rsh);
}

std::tuple<bool, Vec2, float> intersects(const Rect& l, const Rect& r) {
    if(l.x < r.x + r.w && l.x + l.w > r.x && l.y < r.y + r.h && l.y + l.h > r.y) {
        f32 overlapX = std::min(l.x + l.w, r.x + r.w) - std::max(l.x, r.x);
        f32 overlapY = std::min(l.y + l.h, r.y + r.h) - std::max(l.y, r.y);

        Vec2 normal;
        if(overlapX > overlapY) {
            normal = (l.y > r.y) ? Vec2(0.0f, -1.0f) : Vec2(0.0f, 1.0f);
        } else {
            normal = (l.x > r.x) ? Vec2(-1.0f, 0.0f) : Vec2(1.0f, 0.0f);
        }

        return {true, normal, std::min(overlapX, overlapY)};
    }
    return {false, {}, 0};
}

std::tuple<bool, Vec2, float> intersects(const Rect& l, const Circle& r) {
    // AABB check first
    if(r.x > l.x && r.x < l.x + l.w && r.y > l.y && r.y < l.y + l.h) {
        return {true, {}, 0};
    }

    f32 rectMinX = l.x;
    f32 rectMaxX = l.x + l.w;
    f32 rectMinY = l.y;
    f32 rectMaxY = l.y + l.h;

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
        if(auto [suc, normal, depth] = intersects(e, r); suc) {
            return {true, normal, depth};
        }
    }

    return {false, {}, 0};
}

std::tuple<bool, Vec2, float> intersects(const Rect& l, const Line& r) {
    f32 rectMinX = l.x;
    f32 rectMaxX = l.x + l.w;
    f32 rectMinY = l.y;
    f32 rectMaxY = l.y + l.h;

    f32 lineMinX = std::min(r.p1.x, r.p2.x);
    f32 lineMaxX = std::max(r.p1.x, r.p2.x);
    f32 lineMinY = std::min(r.p1.y, r.p2.y);
    f32 lineMaxY = std::max(r.p1.y, r.p2.y);

    f32 overlapX = std::min(rectMaxX, lineMaxX) - std::max(rectMinX, lineMinX);
    f32 overlapY = std::min(rectMaxY, lineMaxY) - std::max(rectMinY, lineMinY);

    // no overlap
    if(overlapX < 0 || overlapY < 0) {
        return {false, {}, 0};
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
        if(auto [suc, normal, depth] = intersects(e, r); suc) {
            return {true, normal, depth};
        }
    }

    return {false, {}, 0};
}

std::tuple<bool, Vec2, float> intersects(const Circle& l, const Rect& r) {
    return intersects(r, l);
}

std::tuple<bool, Vec2, float> intersects(const Circle& l, const Circle& r) {
    auto lrx = pow(l.x - r.x, 2);
    auto lry = pow(l.y - r.y, 2);

    // square distance between centers
    auto squareDistance = lrx + lry;

    auto squareRadius = pow(l.r + r.r, 2);

    return {squareDistance <= squareRadius, {}, 0};
}

std::tuple<bool, Vec2, float> intersects(const Circle& l, const Line& r) {
    Vec2 center = {l.x, l.y};
    Vec2 lineToCenter = center - r.p1;
    Vec2 lineDirection = r.p2 - r.p1;

    f32 numerator = math::dot(lineToCenter, lineDirection);
    f32 denominator = math::dot(lineDirection, lineDirection);

    // line is a point
    if(denominator == 0) {
        return {false, {}, 0};
    }
    f32 t = numerator / denominator;
    t = std::clamp(t, 0.0f, 1.0f);

    Vec2 p = r.p1 + t * lineDirection;

    f32 squareDistance = math::dot(center - p, center - p);
    f32 squareRadius = l.r * l.r;

    return {squareDistance <= squareRadius, {}, 0};
}

std::tuple<bool, Vec2, float> intersects(const Line& l, const Rect& r) {
    return intersects(r, l);
}

std::tuple<bool, Vec2, float> intersects(const Line& l, const Circle& r) {
    return intersects(r, l);
}

std::tuple<bool, Vec2, float> intersects(const Line& l, const Line& r) {
    Vec2 LtoR = r.p1 - l.p1;
    Vec2 lDirection = l.p2 - l.p1;
    Vec2 rDirection = r.p2 - r.p1;

    // relative direction between lines
    f32 numerator = math::cross(LtoR, rDirection);

    f32 denominator = math::cross(lDirection, rDirection);

    // lines are parallel
    if(denominator == 0.0f) {
        return {false, {}, 0};
    }

    f32 t = numerator / denominator;

    if(t >= 0.0f && t <= 1.0f) {
        return {true, {}, 0};
    }

    return {false, {}, 0};
}
