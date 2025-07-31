#include "spell_book.hpp"

#include "../asset_manager.hpp"
#include "../entity.hpp"
#include "../log.hpp"
#include "animation.hpp"
#include "mana.hpp"
#include "owner.hpp"
#include "particle_system.hpp"
#include "spawn.hpp"
#include "spell.hpp"
#include "status_effect.hpp"

void SpellBookComponent::attach() {
    auto am = AssetManager::get();
    for(auto& it : spellFiles_) {
        auto s = am->load<SpellData>(it);
        if(s) {
            addSpell(s);
        }
    }
    autoEquipSpells();
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
        } else {
            ++it;
        }
    }

    if(castedSpell_) {
        // put spell on cooldown if it has one
        if(castedSpell_->cooldown > 0.0f) {
            cooldowns_.emplace(castedSpell_, castedSpell_->cooldown);
        }

        auto statusEffectComponent = entity()->component<StatusEffectComponent>();

        // if stunned during casting
        if(statusEffectComponent && statusEffectComponent->isUnderEffect(SpellEffectType::STUN)) {
            interruptCasting();
            return;
        }

        castDuration_ += dt;
        castProgress_ = castDuration_ / castedSpell_->castTime;

        if(castDuration_ >= castedSpell_->castTime) {
            INFO("[SPELL BOOK]: " + castedSpell_->name + " going off after " +
                 std::to_string(castDuration_) + "s");

            EntityPtr spellEntity = Entity::create(castedSpell_->name, true);
            spellEntity->setTransform(entity()->transform());
            auto spellComponent = std::make_shared<SpellComponent>(castedSpell_);

            // pass caster's tag in case caster dies
            if(castedSpell_->action.type != ActionType::SELF) {
                auto tagComponent = entity()->component<TagComponent>();
                if(tagComponent) {
                    spellComponent->setCasterTag(tagComponent->tag());
                }
            }

            auto ownerComponent = std::make_shared<OwnerComponent>();
            ownerComponent->setOwner(entity());
            spellEntity->addComponent(ownerComponent);

            spellEntity->addComponent(spellComponent);

            if(castedSpell_->requiresComponent(SpellRequirement::GEOMETRY)) {
                auto geometryComponent = std::make_shared<GeometryComponent>();
                auto geometryData = determineGeometry();
                geometryComponent->setGeometryData(geometryData);
                geometryComponent->setTextureFilePath(castedSpell_->textureFilePath);
                spellEntity->addComponent(geometryComponent);
            }

            if(castedSpell_->requiresComponent(SpellRequirement::COLLISION)) {
                auto collisionComponent = std::make_shared<CollisionComponent>();
                auto collisionData = determineCollision();
                collisionComponent->setCollisionShape(collisionData.shape);
                spellEntity->addComponent(collisionComponent);
            }

            if(castedSpell_->requiresComponent(SpellRequirement::ANIMATION)) {
                std::shared_ptr<AnimationComponent> animationComponent =
                    std::make_shared<AnimationComponent>();
                animationComponent->addAnimationFiles(castedSpell_->animationFiles);
                animationComponent->queueAnimation("idle");
                spellEntity->addComponent(animationComponent);
            }

            if(castedSpell_->requiresComponent(SpellRequirement::PARTICLE)) {
                std::shared_ptr<ParticleSystemComponent> particleSystemComponent =
                    std::make_shared<ParticleSystemComponent>();
                particleSystemComponent->addEmitterFiles(castedSpell_->emitterFiles);
                spellEntity->addComponent(particleSystemComponent);
            }

            if(castedSpell_->requiresComponent(SpellRequirement::SPAWN)) {
                std::shared_ptr<SpawnComponent> spawnComponent = std::make_shared<SpawnComponent>();
                spawnComponent->setSpawn(castedSpell_->spawnName, castedSpell_->spawnPrefabFile);
                auto spellEntityTransform = spellEntity->transform();
                spellEntityTransform.position = target_;
                spellEntity->setTransform(spellEntityTransform);
                spellEntity->addComponent(spawnComponent);
            }

            // update mana
            if(castedSpell_->manaCost != 0) {
                auto mana = entity()->component<ManaComponent>();
                if(!mana) {
                    ERROR("[SPELL BOOK]: missing mana component");
                    return;
                }
                mana->reduceMana(castedSpell_->manaCost);
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

    // prevent casting due to being stunned
    auto statusEffectComponent = entity()->component<StatusEffectComponent>();
    if(statusEffectComponent && statusEffectComponent->isUnderEffect(SpellEffectType::STUN)) {
        return;
    }

    if(isSpellInSlotOnCooldown(index)) {
        ERROR_ONCE("[SPELL BOOK]: " + spellSlots_[index]->name + " on cooldown");
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
    if(spell->manaCost != 0) {
        auto mana = entity()->component<ManaComponent>();
        if(!mana) {
            ERROR("[SPELL BOOK]: missing mana component");
            return;
        }

        if(mana->mana().current < static_cast<f32>(spell->manaCost)) {
            auto tagComponent = entity()->component<TagComponent>();
            if(tagComponent->tag() == TagType::PLAYER) {
                INFO("[SPELL BOOK]: not enough mana");
            }
            return;
        }
    }

    INFO("[SPELL BOOK]: caster " + entity()->name() + " casting " + spell->name);

    castedSpell_ = spell;
    target_ = target;
}

void SpellBookComponent::interruptCasting() {
    if(castedSpell_) {
        INFO("[SPELL BOOK]: interrupting casting " + castedSpell_->name + "...");
        castedSpell_ = nullptr;
        castDuration_ = 0.0f;
    }
}

auto SpellBookComponent::spells() const -> const std::vector<std::shared_ptr<SpellData>> {
    return spells_;
}

auto SpellBookComponent::spell(u32 index) -> std::shared_ptr<SpellData> {
    if(index > spells_.size()) {
        return nullptr;
    }
    return spells_[index];
}

void SpellBookComponent::setSlot(u32 index, std::shared_ptr<SpellData> spell) {
    assert(index < spellSlots_.size());
    spellSlots_[index] = spell;
}

auto SpellBookComponent::slots() const -> std::array<std::shared_ptr<SpellData>, 4> {
    return spellSlots_;
}

bool SpellBookComponent::interruptible() const {
    if(!castedSpell_) {
        return false;
    } else {
        return castDuration_ <= castedSpell_->interruptTime;
    }
}

f32 SpellBookComponent::castProgress() const {
    return castProgress_;
}

auto SpellBookComponent::castedSpell() -> std::shared_ptr<SpellData const> const {
    return castedSpell_;
}

bool SpellBookComponent::isCasting() {
    return castedSpell_ ? true : false;
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

u32 SpellBookComponent::availableSpellsCount() const {
    return static_cast<u32>(spells_.size());
}

void SpellBookComponent::autoEquipSpells() {
    for(u32 j = 0; j < spellSlots_.size(); j++) {
        for(u32 i = j; i < spells_.size();) {
            spellSlots_[j] = spells_[i];
            break;
        }
    }
}

auto SpellBookComponent::determineGeometry() -> GeometryData {
    auto geometryData = castedSpell_->geometryData;
    auto maxRange = castedSpell_->maxRange;

    // resolve geometry size based on target position
    if(geometryData.sizeDeterminant == GeometrySizeDeterminant::TARGET) {
        Vec2 castPosition = entity()->transform().position;
        f32 length = Vec2(target_ - castPosition).length();

        // clamp to max range
        length = length > maxRange ? maxRange : length;

        Rect rect = geometryData.rect;
        rect.w = rect.w == 0 ? length : rect.w;
        rect.h = rect.h == 0 ? length : rect.h;
        geometryData.rect = rect;
    }
    return geometryData;
}

auto SpellBookComponent::determineCollision() -> CollisionData {
    auto collisionData = castedSpell_->collisionData;
    auto maxRange = castedSpell_->maxRange;
    Vec2 castPosition = entity()->transform().position;
    f32 length = Vec2(castPosition - target_).length();

    // clamp to max range
    length = length > maxRange ? maxRange : length;

    // resolve collision object based on target position
    if(collisionData.sizeDeterminant == CollisionSizeDeterminant::TARGET) {
        if(collisionData.shape.shape() == Shape::RECT) {
            Rect rect = std::get<Rect>(collisionData.shape);
            rect.x = castPosition.x + rect.x;
            rect.y = castPosition.y + rect.y;
            rect.w = rect.w == 0 ? length : rect.w;
            rect.h = rect.h == 0 ? length : rect.h;

            Vec2 pivot(rect.w + rect.x, rect.h + rect.y);
            rect.rotate(entity()->transform().rotation, &pivot);
            collisionData.shape = rect;
        }

        if(collisionData.shape.shape() == Shape::LINE) {
            Line line = std::get<Line>(collisionData.shape);
            line.p1.x = castPosition.x;
            line.p1.y = castPosition.y;
            line.p2.x = castPosition.x + length;
            line.p2.y = castPosition.y;

            line.rotate(entity()->transform().rotation, &line.p1);
            collisionData.shape = line;
        }

        if(collisionData.shape.shape() == Shape::CIRCLE) {
            Circle circle = std::get<Circle>(collisionData.shape);
            Vec2 castDirection = Vec2(target_ - castPosition);
            Vec2 newTargetPosition = castDirection.normalized();
            newTargetPosition = newTargetPosition * length;
            circle.x = newTargetPosition.x + castPosition.x;
            circle.y = newTargetPosition.y + castPosition.y;
            collisionData.shape = circle;
        }
    }
    return collisionData;
}
