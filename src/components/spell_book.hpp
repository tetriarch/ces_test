#pragma once

#include "../component.hpp"
#include "../math.hpp"
#include "../utils.hpp"
class SpellData;

class SpellBookComponent : public Component<SpellBookComponent> {

public:
    void attach() override;
    void addSpell(const std::shared_ptr<SpellData> spellData);
    void addSpellFile(const std::string& filePath);
    void castSpell(u32 index, EntityPtr caster, const Vec2& target);
    auto spells() const -> const std::vector<std::shared_ptr<SpellData>>;
    void setSlot(u32 index, std::shared_ptr<SpellData> spell);

private:
    std::array<std::shared_ptr<SpellData>, 4> spellSlots_;
    std::vector<std::shared_ptr<SpellData>> spells_;
    std::vector<std::string> spellFiles_;
};