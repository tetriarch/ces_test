#pragma once

#include "../component.hpp"
#include "../math.hpp"
#include "../utils.hpp"
class SpellData;

class SpellBookComponent : public Component<SpellBookComponent> {

public:
    auto describe() -> std::string override;
    void addSpell(const std::shared_ptr<SpellData> spellData);
    void castSpell(const std::shared_ptr<SpellData> spell, EntityPtr caster, const Vec2& target);
    auto spells() const -> const std::vector<std::shared_ptr<SpellData>>;

private:
    std::vector<std::shared_ptr<SpellData>> spells_;
};