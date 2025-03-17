#include "../log.hpp"

#include "components.hpp"
#include "spell_book.hpp"
#include "../asset_manager.hpp"

void SpellBookComponent::attach() {

    auto am = AssetManager::get();
    for(auto& it : spellFiles_) {
        auto s = am->load<SpellData>(it);
        if(s) {
            addSpell(s);
        }
    }
}

void SpellBookComponent::update(f32 dt) {

    // update cooldowns
    for(auto&& cd : cooldowns_) {
        cd.second -= dt;
    }

    // erase cooldown if it reached 0
    for(auto it = cooldowns_.begin(); it != cooldowns_.end();) {
        if(it->second <= 0.0f) {
            it = cooldowns_.erase(it);
        }
        else {
            ++it;
        }
    }

    if(castedSpell_) {

        castDuration_ += dt;
        castProgress_ = castDuration_ / castedSpell_->castTime;
        //TODO: Implement on status effect interruption(stun, freeze...), cooldown

        if(castDuration_ >= castedSpell_->castTime) {
            INFO("[SPELL BOOK]: " + castedSpell_->name + " going off after " + std::to_string(castDuration_) + "s");

            EntityPtr spellEntity = Entity::create(castedSpell_->name, true);
            spellEntity->setTransform(entity()->transform());
            spellEntity->addComponent(std::make_shared<CollisionComponent>());
            spellEntity->addComponent(std::make_shared<SpellComponent>(castedSpell_));

            // add texture if described
            if(!castedSpell_->textureFilePath.empty()) {
                std::shared_ptr<TextureComponent> textureComponent = std::make_shared<TextureComponent>();
                textureComponent->setFilePath(castedSpell_->textureFilePath);
                spellEntity->addComponent(textureComponent);
            }

            // update mana
            auto mana = entity()->component<ManaComponent>();
            if(!mana) {
                ERROR("[SPELL BOOK]: missing mana component");
                return;
            }
            auto currentMana = mana->mana();
            currentMana.current -= castedSpell_->manaCost;
            mana->setMana(currentMana);

            // put spell on cooldown if it has one
            if(castedSpell_->cooldown > 0.0f) {
                cooldowns_.emplace(spellSlotIndex(castedSpell_), castedSpell_->cooldown);
            }

            castedSpell_ = nullptr;
            castProgress_ = 0.0f;
            castDuration_ = 0.0f;
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

    if(isSpellSlotOnCooldown(index)) {
        INFO("[SPELL BOOK]: " + spellSlots_[index]->name + " on cooldown");
        return;
    }

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

    INFO("[SPELL BOOK]: caster " + entity()->name() + " casting " + spell->name);

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

s32 SpellBookComponent::spellSlotIndex(std::shared_ptr<SpellData> spell) {

    for(u32 index = 0; index < spellSlots_.size(); index++) {
        if(spell == spellSlots_[index]) {
            return static_cast<s32>(index);
        }
    }
    return -1;
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

bool SpellBookComponent::isSpellSlotOnCooldown(u32 index, f32* cooldown, f32* progress) {

    if(auto it = cooldowns_.find(index); it != cooldowns_.end()) {
        if(cooldown) {
            *cooldown = it->second;
        }
        if(progress) {
            *progress = it->second / spellSlots_[it->first]->cooldown;
        }
        return true;
    }
    return false;
}
