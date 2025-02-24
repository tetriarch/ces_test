#include "spell.hpp"
#include "components.hpp"

SpellComponent::SpellComponent(std::shared_ptr<SpellData> spellData) {
    spellData_ = spellData;
}

void SpellComponent::cast(EntityPtr& source, Vec2 target) {

    EntityPtr spell = Entity::create(spellData_->name);
    spell->setTransform(source->getTransform());
    spell->addComponent(std::make_shared<CollisionComponent>());
    // need to make sure that the spell flying has it's own data.
    spell->addComponent(std::make_shared<SpellComponent>(*this));
    source->addChild(spell);
}

auto SpellComponent::describe() -> std::string {
    return "Name: " + spellData_->name + "\n -" +
        " Cast Time: " + std::to_string(spellData_->castTime) +
        " Interrupt Time: " + std::to_string(spellData_->interruptTime) +
        " Mana Cost: " + std::to_string(spellData_->manaCost) +
        " Cooldown:" + std::to_string(spellData_->cooldown);
}
