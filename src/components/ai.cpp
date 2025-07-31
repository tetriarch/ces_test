#include "ai.hpp"

#include "../entity.hpp"
#include "../renderer.hpp"
#include "life.hpp"
#include "spell_book.hpp"
#include "status_effect.hpp"
#include "velocity.hpp"

const f32 AGGRO_LOSS_MULTIPLAYER = 2.0f;
const f32 COMBAT_ENTRY_COOLDOWN = 1.0f;
const f32 DEFAULT_IDLE_TIMER = 1.5f;

AIComponent::AIComponent()
    : idleTimer_(DEFAULT_IDLE_TIMER),
      aggroRadius_(0.0f),
      combatEntryCooldown_(COMBAT_ENTRY_COOLDOWN) {
}

void AIComponent::attach() {
    auto velocityComponent = entity()->component<VelocityComponent>();
    if(velocityComponent) {
        velocityComponent->setAIControl(true);
    }
}

void AIComponent::update(const f32 dt) {
    // no update if stunned
    auto statusEffectComponent = entity()->component<StatusEffectComponent>();
    if(statusEffectComponent) {
        if(statusEffectComponent->isUnderEffect(SpellEffectType::STUN)) {
            return;
        }
    }

    switch(mode_) {
        case AIMode::IDLE:
            updateIdle(dt);
            break;
        case AIMode::COMBAT:
            updateCombat(dt);
            break;
        default:
            break;
    }
}

void AIComponent::render(std::shared_ptr<Renderer> renderer) {
#ifdef DEBUG
    auto geometryComponent = entity()->component<GeometryComponent>();
    if(geometryComponent) {
        auto t = combatEntryCooldown_ / COMBAT_ENTRY_COOLDOWN;
        Rect rect = geometryComponent->rect();

        Rect maxCombatCooldown = rect;
        maxCombatCooldown.y -= 15;
        maxCombatCooldown.h = 5;

        Rect currentCombatCooldown;
        currentCombatCooldown = maxCombatCooldown;
        currentCombatCooldown.w *= t;

        renderer->queueRenderFilledRect(Strata::UI, maxCombatCooldown, 32, 32, 0, 255);
        renderer->queueRenderFilledRect(Strata::UI, currentCombatCooldown, 128, 128, 0, 255);
    }
#endif
}

void AIComponent::setAggroRadius(f32 radius) {
    aggroRadius_ = radius;
}

EntityHandle AIComponent::target() {
    return target_;
}

void AIComponent::setTarget(EntityPtr target) {
    target_ = target;
}

bool AIComponent::isInMode(AIMode mode) {
    if(mode_ == mode) {
        return true;
    }
    return false;
}

void AIComponent::resolveEffectApplier(EntityPtr applier) {
    auto tagComponent = entity()->component<TagComponent>();
    auto aTagComponent = applier->component<TagComponent>();

    if(!tagComponent) {
        ERROR("[AI]: " + entity()->name() + " does not have tag component");
        return;
    }
    if(!aTagComponent) {
        ERROR("[AI]: " + applier->name() + " does not have tag component");
        return;
    }
    // if applier is hostile - enter combat
    if(tagComponent->isHostile(aTagComponent->tag())) {
        if(isInMode(AIMode::IDLE)) {
            enterCombat(applier);
            INFO("[AI]: " + entity()->name() + " entering combat - under attack by " +
                 applier->name());
        }
    }
}

void AIComponent::updateIdle(const f32 dt) {
    auto velocityComponent = entity()->component<VelocityComponent>();
    auto transform = entity()->transform();

    if(idleTimer_ <= 0) {
        Vec2 direction;
        direction.x = rng_.getFloat(-1.0f, 1.0f);
        direction.y = rng_.getFloat(-1.0f, 1.0f);
        if(direction.length() >= 0.2f) {
            if(velocityComponent) {
                velocityComponent->setMotion({direction.x, direction.y});
                transform.rotation = angleFromDirection(direction);
                entity()->setTransform(transform);
            }
        } else {
            // clear motion on short direction length
            if(velocityComponent) {
                velocityComponent->clearMotion();
            }
        }
        idleTimer_ = rng_.getFloat(0.5f, 1.5f);
    } else {
        idleTimer_ -= dt;
    }

    // populate friends and foes lists
    collectEntitiesInRange();

    // update combat cooldown
    if(!canEnterCombat()) {
        combatEntryCooldown_ -= dt;
    }
    // enemy in range - comabt
    if(enemyInRange()) {
        if(canEnterCombat()) {
            enterCombat();
            INFO("[AI]: " + entity()->name() + " entering combat - enemies in range");
            return;
        }
    }

    // ally in range in combat - combat
    if(allyInRange()) {
        for(auto& a : alliesInRange_) {
            auto ally = a.lock();
            auto allyAIComponent = ally->component<AIComponent>();
            // if ally is in combat
            if(allyAIComponent) {
                if(allyAIComponent->isInMode(AIMode::COMBAT) && !isInMode(AIMode::COMBAT)) {
                    // if we can fight
                    if(canEnterCombat()) {
                        enterCombat(allyAIComponent->target().lock());
                        INFO("[AI]: " + entity()->name() + " entering combat - ally requests help");
                    }
                }
            }
        }
    }
}

void AIComponent::updateCombat(const f32 dt) {
    auto target = target_.lock();
    auto transform = entity()->transform();

    if(target) {
        auto targetLifeComponent = target->component<LifeComponent>();
        if(targetLifeComponent && targetLifeComponent->isDead()) {
            collectEntitiesInRange();
            if(!enemyInRange()) {
                leaveCombat();
                INFO("[AI]: " + entity()->name() + " leaving combat - enemies are dead");
            } else {
                // re-enter combat (overrides cd)
                enterCombat();
            }
        }

        auto targetTransform = target->transform();
        Vec2 targetDirection = targetTransform.position - transform.position;

        auto velocityComponent = entity()->component<VelocityComponent>();
        if(velocityComponent) {
            velocityComponent->setMotion(targetDirection);
            transform.rotation = angleFromDirection(targetDirection);
            entity()->setTransform(transform);
        }

        auto spellBookComponent = entity()->component<SpellBookComponent>();
        if(spellBookComponent) {
            u32 spellCount = spellBookComponent->availableSpellsCount();
            if(spellCount > 0) {
                u32 spellIndex = rng_.getUnsigned(0, spellCount - 1);
                auto spell = spellBookComponent->spell(spellIndex);
                if(spell && spell->action.type == ActionType::SPAWN) {
                    auto geometryComponent = entity()->component<GeometryComponent>();
                    if(geometryComponent) {
                        auto rect = geometryComponent->rect();
                        auto range = rng_.getFloat(-spell->maxRange, spell->maxRange);
                        if(range > 0) {
                            range -= std::min(rect.w, rect.h);
                        } else {
                            range += std::min(rect.w, rect.h);
                        }

                        targetDirection.x = transform.position.x + range;
                        targetDirection.y = transform.position.y + range;
                    }
                }

                if(!spellBookComponent->isCasting()) {
                    spellBookComponent->castSpell(spellIndex, targetDirection);
                }
            }
        }
        if(Vec2(targetTransform.position - transform.position).length() >
            aggroRadius_ * AGGRO_LOSS_MULTIPLAYER) {
            leaveCombat();
            INFO("[AI]: " + entity()->name() + " leaving combat - target outside range");
        }
    }
}

void AIComponent::collectEntitiesInRange() {
    enemiesInRange_.clear();
    alliesInRange_.clear();

    auto tagComponents = TagComponent::trackedComponents();
    auto transform = entity()->transform();

    auto tagComponent = entity()->component<TagComponent>();
    if(!tagComponent) {
        ERROR_ONCE("[AI]: missing tag component");
        return;
    }

    for(auto& weakTagComponent : tagComponents) {
        auto eTagComponent = weakTagComponent.lock();
        if(!eTagComponent || eTagComponent == tagComponent) continue;

        auto ePtr = eTagComponent->entity();
        if(!ePtr) {
            continue;
        }

        auto eTransform = ePtr->transform();
        f32 distance = Vec2(eTransform.position - transform.position).length();
        if(distance < aggroRadius_) {
            if(tagComponent->isHostile(eTagComponent->tag())) {
                if(ePtr->isActive()) {
                    enemiesInRange_.emplace_back(ePtr);
                }
            }
            if(tagComponent->isFriendly(eTagComponent->tag())) {
                if(ePtr->isActive()) {
                    alliesInRange_.emplace_back(ePtr);
                }
            }
        }
    }
}

bool AIComponent::enemyInRange() const {
    return !enemiesInRange_.empty();
}

bool AIComponent::allyInRange() const {
    return !alliesInRange_.empty();
}

bool AIComponent::canEnterCombat() {
    return combatEntryCooldown_ <= 0.0f;
}

void AIComponent::enterCombat(EntityPtr target) {
    mode_ = AIMode::COMBAT;
    combatEntryCooldown_ = COMBAT_ENTRY_COOLDOWN;
    if(target) {
        target_ = target;
        return;
    }
    // no target - try pick one
    u32 targetIndex = rng_.getUnsigned(0, enemiesInRange_.size() - 1);
    target_ = enemiesInRange_[targetIndex];
}

void AIComponent::leaveCombat() {
    mode_ = AIMode::IDLE;
    idleTimer_ = 0;
    auto spellBookComponent = entity()->component<SpellBookComponent>();
    if(spellBookComponent) {
        if(spellBookComponent->isCasting()) {
            spellBookComponent->interruptCasting();
        }
    }
}
