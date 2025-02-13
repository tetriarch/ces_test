#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../math.hpp"

class CollisionComponent : public Component<CollisionComponent> {

public:
    void setCollisionBox(const Rect& rect);
    const Rect getCollisionBox() const { return rect_; }
    bool checkCollision(EntityPtr target) {
        auto components = target->getComponents();
        for(auto& c : components) {
            if(c->getComponentType() == typeid(CollisionComponent)) {
                auto targetCollisionComp = static_cast<CollisionComponent*>(c.get());
                return checkAABBIntersection(rect_, targetCollisionComp->getCollisionBox());
                // TODO:figure out how to make interaction between produced colllision and hit dmg
                // return accordingly
            }
        }
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