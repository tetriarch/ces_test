#include "spell.hpp"
#include "components.hpp"

SpellComponent::SpellComponent(std::shared_ptr<SpellData> spellData) {
    spellData_ = spellData;
}



auto SpellComponent::describe() -> std::string {
    return "Name: " + spellData_->name + " -" +
        " Cast Time: " + std::to_string(spellData_->castTime) +
        " Interrupt Time: " + std::to_string(spellData_->interruptTime) +
        " Mana Cost: " + std::to_string(spellData_->manaCost) +
        " Cooldown:" + std::to_string(spellData_->cooldown);
}
