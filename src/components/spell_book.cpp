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

void SpellBookComponent::update() {

    if(castedSpell_) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<f32> elapsed = currentTime - castStart_;
        if(elapsed.count() >= castedSpell_->castTime) {
            INFO("[SPELL BOOK]: " + castedSpell_->name + " going off after " + std::to_string(elapsed.count()) + "s");

            EntityPtr spellEntity = Entity::create(castedSpell_->name);
            spellEntity->setTransform(entity()->transform());
            spellEntity->addComponent(std::make_shared<CollisionComponent>());
            spellEntity->addComponent(std::make_shared<SpellComponent>(castedSpell_));
            castedSpell_ = nullptr;
            entity()->root()->addChild(spellEntity);
        }
    }
}

void SpellBookComponent::addSpell(const std::shared_ptr<SpellData> spellData) {

    spells_.push_back(spellData);
}

void SpellBookComponent::addSpellFile(const std::string& filePath) {
    spellFiles_.push_back(filePath);
}

void SpellBookComponent::castSpell(u32 index, const Vec2& target) {

    assert(index < spellSlots_.size());

    auto spell = spellSlots_[index];
    if(!spell) {
        return;
    }

    INFO("[SPELL BOOK]: caster " + entity()->name() + " casting " + spell->name + " at target [" + std::to_string(target.x) + "," + std::to_string(target.y) + "]");
    castedSpell_ = spell;
    castStart_ = std::chrono::high_resolution_clock::now();
}

auto SpellBookComponent::spells() const -> const std::vector<std::shared_ptr<SpellData>> {

    return spells_;
}

void SpellBookComponent::setSlot(u32 index, std::shared_ptr<SpellData> spell) {

    assert(index < spellSlots_.size());
    spellSlots_[index] = spell;
}
