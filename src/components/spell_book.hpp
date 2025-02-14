#pragma once

#include "../component.hpp"
#include "../spell_manager.hpp"

class SpellBookComponent : public Component<SpellBookComponent> {
public:
    SpellBookComponent(SpellManager* spellManager) { spellManager_ = spellManager; }
    auto describe() -> std::string override { return "I cast spells"; }
    void addSpell(const std::string& spellName) { spells_.push_back(spellName); }
    void castSpell(const std::string& spellName) {
        for(auto& s : spells_) {
            if(s == spellName) {
                spellManager_->cast(spellName, getParent());
            }
        }
    }
    auto getSpells() const -> const std::vector<std::string>* { return &spells_; }
private:
    std::vector<std::string> spells_;
    SpellManager* spellManager_;
};