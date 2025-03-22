#include "spell.hpp"
#include "components.hpp"

SpellComponent::SpellComponent(std::shared_ptr<SpellData> spellData) {

    spellData_ = spellData;
    lifeTime_ = spellData->lifeTime;
}

void SpellComponent::update(f32 dt) {

    for(auto&& action : spellData_->actions) {
        switch(action.type) {
            case ActionType::PROJECTILE: {
                action.motion->apply(entity(), dt);
                break;
            }
            case ActionType::AOE: break;
            case ActionType::ARC: break;
            case ActionType::BEAM: break;
            case ActionType::UNKNOWN: break;
        }
    }

    lifeTime_ -= dt;
}

void SpellComponent::postUpdate(f32 dt) {

    if(lifeTime_ <= 0.0f) {
        entity()->parent()->queueRemoveChild(entity());
    }
}
