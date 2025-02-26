#include "spell_book.hpp"
#include "components.hpp"

auto SpellBookComponent::describe() -> std::string {
    return "I cast spells";
}

void SpellBookComponent::addSpell(const std::shared_ptr<SpellData> spellData) {

    spells_.push_back(spellData);
}

void SpellBookComponent::castSpell(const std::shared_ptr<SpellData> spell, EntityPtr caster, const Vec2& target) {

    std::cout << "[SPELL BOOK]: casting " << spell->name << std::endl;

    EntityPtr spellEntity = Entity::create(spell->name);
    spellEntity->setTransform(caster->getTransform());
    spellEntity->addComponent(std::make_shared<CollisionComponent>());
    // need to make sure that the spellEntity flying has it's own data.
    spellEntity->addComponent(std::make_shared<SpellComponent>(spell));
    caster->addChild(spellEntity);
}

auto SpellBookComponent::getSpells() const -> const std::vector<std::shared_ptr<SpellData>> {
    return spells_;
}
