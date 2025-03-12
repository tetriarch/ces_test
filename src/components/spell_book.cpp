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

        //TODO: Implement on status effect interruption(stun, freeze...)

        if(elapsed.count() >= castedSpell_->castTime) {
            INFO("[SPELL BOOK]: " + castedSpell_->name + " going off after " + std::to_string(elapsed.count()) + "s");

            EntityPtr spellEntity = Entity::create(castedSpell_->name);
            spellEntity->setTransform(entity()->transform());
            spellEntity->addComponent(std::make_shared<CollisionComponent>());
            spellEntity->addComponent(std::make_shared<SpellComponent>(castedSpell_));

            // update mana
            auto mana = entity()->component<ManaComponent>();
            auto currentMana = mana->mana();
            currentMana.current -= castedSpell_->manaCost;
            mana->setMana(currentMana);

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

    // if allready casting, interrupt if possible
    if(castedSpell_) {
        interruptCasting();
    }

    auto spell = spellSlots_[index];
    if(!spell) {
        return;
    }

    // check if we have enough mana
    auto mana = entity()->component<ManaComponent>();
    if(!mana) {
        ERROR("[SPELL BOOK]: missing mana component");
        return;
    }

    if(mana->mana().current < spell->manaCost) {
        INFO("Not enough mana");
        return;
    }

    INFO("[SPELL BOOK]: caster " + entity()->name() + " casting " + spell->name + " at target [" + std::to_string(target.x) + "," + std::to_string(target.y) + "]");

    castedSpell_ = spell;
    castStart_ = std::chrono::high_resolution_clock::now();
}

void SpellBookComponent::interruptCasting() {

    std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();

    std::chrono::duration<f32> elapsed = current - castStart_;
    if(elapsed.count() <= castedSpell_->interruptTime) {
        INFO("[SPELL BOOK]: interrupting casting " + castedSpell_->name + "...");
        castedSpell_ = nullptr;
        castStart_ = std::chrono::high_resolution_clock::time_point();
    }
}

auto SpellBookComponent::spells() const -> const std::vector<std::shared_ptr<SpellData>> {

    return spells_;
}

void SpellBookComponent::setSlot(u32 index, std::shared_ptr<SpellData> spell) {

    assert(index < spellSlots_.size());
    spellSlots_[index] = spell;
}
