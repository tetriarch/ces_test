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

void SpellBookComponent::update(f32 dt) {

    if(castedSpell_) {
        castDuration_ += dt;
        castProgress_ = castDuration_ / castedSpell_->castTime;
        //TODO: Implement on status effect interruption(stun, freeze...)

        if(castDuration_ >= castedSpell_->castTime) {
            castProgress_ = 0.0f;
            INFO("[SPELL BOOK]: " + castedSpell_->name + " going off after " + std::to_string(castDuration_) + "s");

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
            castDuration_ = 0.0f;
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

    if(castedSpell_) {
        if(!interruptible()) {
            return;
        }
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
}

void SpellBookComponent::interruptCasting() {

    INFO("[SPELL BOOK]: interrupting casting " + castedSpell_->name + "...");
    castedSpell_ = nullptr;
    castDuration_ = 0.0f;
}

auto SpellBookComponent::spells() const -> const std::vector<std::shared_ptr<SpellData>> {

    return spells_;
}

void SpellBookComponent::setSlot(u32 index, std::shared_ptr<SpellData> spell) {

    assert(index < spellSlots_.size());
    spellSlots_[index] = spell;
}

bool SpellBookComponent::interruptible() const {

    if(!castedSpell_) {
        return false;
    }
    else {
        return castDuration_ <= castedSpell_->interruptTime;
    }
}

f32 SpellBookComponent::castProgress() const {
    return castProgress_;
}

auto SpellBookComponent::castedSpell() -> std::shared_ptr<SpellData const> const {
    return castedSpell_;
}
