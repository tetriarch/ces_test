#include "../log.hpp"

#include "components.hpp"
#include "spell_book.hpp"
#include "../asset_manager.hpp"

void SpellBookComponent::attach() {

    auto am = AssetManager::get();
    for(auto& it : spellFiles_) {
        auto s = am->load<SpellData>(it);
        addSpell(s);
    }
}

void SpellBookComponent::addSpell(const std::shared_ptr<SpellData> spellData) {

    spells_.push_back(spellData);
}

void SpellBookComponent::addSpellFile(const std::string& filePath) {
    spellFiles_.push_back(filePath);
}

void SpellBookComponent::castSpell(u32 index, EntityPtr caster, const Vec2& target) {

    assert(index >= 0 && index < spellSlots_.size());

    auto spell = spellSlots_[index];
    if(!spell) {
        ERROR("spell is nullptr");
        return;
    }

    INFO("[SPELL BOOK]: casting " + spell->name);

    EntityPtr spellEntity = Entity::create(spell->name);
    spellEntity->setTransform(caster->transform());
    spellEntity->addComponent(std::make_shared<CollisionComponent>());
    spellEntity->addComponent(std::make_shared<SpellComponent>(spell));
    caster->root()->addChild(spellEntity);
}

auto SpellBookComponent::spells() const -> const std::vector<std::shared_ptr<SpellData>> {

    return spells_;
}

void SpellBookComponent::setSlot(u32 index, std::shared_ptr<SpellData> spell) {

    assert(index >= 0 && index < spellSlots_.size());
    spellSlots_[index] = spell;
}
