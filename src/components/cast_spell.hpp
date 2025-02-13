#pragma once

#include "../component.hpp"

class CastSpellComponent : public Component<CastSpellComponent> {
public:
    auto describe() -> std::string override { return "I cast spells"; }
    void addSpell(const std::string& spellName) { spells_.push_back(spellName); }
    void castSpell(const std::string& spellName) {
        for(auto& s : spells_) {
            if(s == spellName) {
                // TODO: SpellManager::cast(spellName, getParent());
            }
        }
    }
    auto getSpells() const -> const std::vector<std::string>* { return &spells_; }
private:
    std::vector<std::string> spells_;
};