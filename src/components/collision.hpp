#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../math.hpp"

class CollisionComponent : public Component<CollisionComponent> {

public:
    void setCollisionBox(const Rect& rect) { rect_ = rect; }
    const Rect collisionBox() const { return rect_; }
    bool checkCollision(EntityPtr target) {
        auto components = target->components();
        for(auto& c : components) {
            if(c->componentType() == typeid(CollisionComponent)) {
                auto targetCollisionComp = dynamic_cast<CollisionComponent*>(c.get());
                if(checkAABBIntersection(rect_, targetCollisionComp->collisionBox())) {
                    return true;
                }
            }
        }
        return false;
    }

private:
    bool checkAABBIntersection(const Rect& a, const Rect& b) {
        return(a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
    }
    Rect rect_;
};