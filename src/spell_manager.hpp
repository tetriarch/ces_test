#pragma once

#include "entity.hpp"

struct FireballSpell {};
struct FrostBoltSpell {};
struct LightningBoltSpell {};
struct FrostNovaSpell {};

class SpellManager {

public:
    SpellManager(EntityPtr scene);
    EntityPtr cast(const std::string& spellName, Entity* caster);

    template<typename T>
    EntityPtr cast(Entity* caster);

private:
    std::unordered_map<std::string, std::function<EntityPtr(Entity*)>> spells_;
    std::weak_ptr<Entity> scene_;
};

template<>
EntityPtr SpellManager::cast<FireballSpell>(Entity* caster);

template<>
EntityPtr SpellManager::cast<FrostBoltSpell>(Entity* caster);

template<>
EntityPtr SpellManager::cast<LightningBoltSpell>(Entity* caster);

template<>
EntityPtr SpellManager::cast<FrostNovaSpell>(Entity* caster);

