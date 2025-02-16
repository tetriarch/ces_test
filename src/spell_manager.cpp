#include "components/components.hpp"
#include "spell_manager.hpp"

SpellManager::SpellManager(EntityPtr scene) {


    // NOTE: There should be some way to drive it with data - parser to load spells and system that adds it in
    // just unsure how, since not all spells are done same way, some just improve stats like movement speed
    // some spells shoot multiple projectiles, or create energy shield etc

    scene_ = scene;
    spells_.emplace("Fireball", [&](Entity* caster) { return cast<FireballSpell>(caster);});
    spells_.emplace("Frost bolt", [&](Entity* caster) { return cast<FrostBoltSpell>(caster);});
    spells_.emplace("Lightning bolt", [&](Entity* caster) { return cast<LightningBoltSpell>(caster);});
    spells_.emplace("Frost nova", [&](Entity* caster) { return cast<FrostNovaSpell>(caster);});
}

EntityPtr SpellManager::cast(const std::string& spellName, Entity* caster) {

    auto it = spells_.find(spellName);
    if(it != spells_.end()) {
        return it->second(caster);
    }
    else {
        std::cerr << "unknown spell" << std::endl;
        return nullptr;
    }
}

template<>
EntityPtr SpellManager::cast<FireballSpell>(Entity* caster) {

    EntityPtr fireball = Entity::create("fireball");
    fireball->setTransform(caster->getTransform());

    if(!scene_.expired()) {
        scene_.lock()->addChild(fireball);
    }
    else {
        std::cerr << "[SPELL MANAGER]: reference to scene has expired" << std::endl;
        return nullptr;
    }

    auto velocity = std::make_shared<VelocityComponent>();

    //NOTE: more hardcoded stuff, this should be data driven via parser, 
    //dmg should also scale with equipment, talents skill level, might need outside dmg deal system that calculates actual output
    velocity->setDirection({1, 0});
    velocity->setSpeed(1.0f);
    fireball->addComponent(std::make_shared<VelocityComponent>());

    auto collision = std::make_shared<Collision>();
    Vec2 fireballPosition = fireball->getTransform().position;
    collision->setCollisionBox({fireballPosition.x, fireballPosition.y, 1, 1});
    fireball->addComponent(collision);

    auto dmg = std::make_shared<Damage>();
    dmg->setDamage(10, 50);
    fireball->addComponent(dmg);
    return fireball;
}

template<>
EntityPtr SpellManager::cast<FrostBoltSpell>(Entity* caster) {
    //TODO: Frost Bolt
    std::cout << "casting frost bolt" << std::endl;
    return nullptr;
}

template<>
EntityPtr SpellManager::cast<LightningBoltSpell>(Entity* caster) {
    //TODO: Lightning Bolt
    std::cout << "casting lightning bolt" << std::endl;
    return nullptr;
}

template<>
EntityPtr SpellManager::cast<FrostNovaSpell>(Entity* caster) {
    //TODO: Frost Nova
    std::cout << "casting frost nova" << std::endl;
    return nullptr;
}