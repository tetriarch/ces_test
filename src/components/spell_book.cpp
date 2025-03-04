#include "../log.hpp"

#include "components.hpp"
#include "spell_book.hpp"

auto SpellBookComponent::describe() -> std::string {
    return "I cast spells";
}

void SpellBookComponent::addSpell(const std::shared_ptr<SpellData> spellData) {

    spells_.push_back(spellData);
}

void SpellBookComponent::castSpell(const std::shared_ptr<SpellData> spell, EntityPtr caster, const Vec2& target) {

    if(!spell) {
        ERROR("spell is nullptr");
        return;
    }

    std::cout << "[SPELL BOOK]: casting " << spell->name << std::endl;

    EntityPtr spellEntity = Entity::create(spell->name);
    spellEntity->setTransform(caster->transform());
    spellEntity->addComponent(std::make_shared<CollisionComponent>());
    spellEntity->addComponent(std::make_shared<SpellComponent>(spell));
    // cstyle cast to get rid of const
    Entity* root = (Entity*)caster->root();
    root->addChild(spellEntity);
}

auto SpellBookComponent::spells() const -> const std::vector<std::shared_ptr<SpellData>> {
    return spells_;
}
