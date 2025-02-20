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

private:
    std::unordered_map<std::string, std::function<EntityPtr(Entity*)>> spells_;
    std::weak_ptr<Entity> scene_;
};


