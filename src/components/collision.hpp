#pragma once

#include "../component.hpp"
#include "../entity.hpp"
#include "../math.hpp"

class Collision : public Component<Collision> {

public:
    void setCollisionBox(const Rect& rect) { rect_ = rect; }
    const Rect getCollisionBox() const { return rect_; }

    bool checkCollision(EntityPtr target) {
        auto components = target->getComponents();
        for(auto& c : components) {
            if(c->getComponentType() == typeid(Collision)) {
                auto targetCollisionComp = dynamic_cast<Collision*>(c.get());
                if(checkAABBIntersection(rect_, targetCollisionComp->getCollisionBox())) {
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