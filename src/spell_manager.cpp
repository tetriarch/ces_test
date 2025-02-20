#include "components/components.hpp"
#include "spell_manager.hpp"

SpellManager::SpellManager(EntityPtr scene) {

    scene_ = scene;
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