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

            std::shared_ptr<GeometryComponent> geometryComponent = std::make_shared<GeometryComponent>();
            Rect rect;
            // resolve dynamic size 
            if(castedSpell_->geometry.type == GeometryType::DYNAMIC) {
                Vec2 castPosition = entity()->transform().position;
                f32 length = Vec2(castPosition - target_).length();
                length = length > castedSpell_->maxDistance ? castedSpell_->maxDistance : length;
                rect = castedSpell_->geometry.rect;
                rect.w = rect.w == 0 ? length : rect.w;
                rect.h = rect.h == 0 ? length : rect.h;
            }
            // static size
            else {
                rect = castedSpell_->geometry.rect;
            }
            geometryComponent->setRect(rect);
            geometryComponent->setTextureFilePath(castedSpell_->textureFilePath);
            spellEntity->addComponent(geometryComponent);


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
                cooldowns_.emplace(castedSpell_, castedSpell_->cooldown);
            }

            castedSpell_ = nullptr;
            castProgress_ = 0.0f;
            castDuration_ = 0.0f;
            target_ = {0.0f, 0.0f};
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

    if(isSpellInSlotOnCooldown(index)) {
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
    target_ = target;
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

bool SpellBookComponent::isSpellInSlotOnCooldown(u32 index, f32* cooldown, f32* progress) {

    assert(index < spellSlots_.size());

    auto spell = spellSlots_[index];

    if(auto it = cooldowns_.find(spell); it != cooldowns_.end()) {
        if(cooldown) {
            *cooldown = it->second;
        }
        if(progress) {
            *progress = it->second / spellSlots_[index]->cooldown;
        }
        return true;
    }

    return false;
}

bool SpellBookComponent::isSpellOnCooldown(std::shared_ptr<SpellData> spell) {

    if(auto it = cooldowns_.find(spell); it != cooldowns_.end()) {
        return true;
    }
    return false;
}
