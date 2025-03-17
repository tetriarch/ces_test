#pragma once

#include "../component.hpp"
#include "../math.hpp"
#include "../utils.hpp"

class SpellData;

class SpellBookComponent : public Component<SpellBookComponent> {

public:
    void attach() override;
    void update(const f32 dt) override;
    void addSpell(const std::shared_ptr<SpellData> spellData);
    void addSpellFile(const std::string& filePath);
    void castSpell(u32 index, const Vec2& target);
    auto spells() const -> const std::vector<std::shared_ptr<SpellData>>;
    void setSlot(u32 index, std::shared_ptr<SpellData> spell);
    bool interruptible() const;
    void interruptCasting();
    f32 castProgress() const;
    f32 cooldownProgress(u32 index) const;
    auto castedSpell() -> std::shared_ptr<SpellData const> const;
    bool isSpellSlotOnCooldown(u32 index, f32* cooldown = nullptr, f32* progress = nullptr);

private:
    s32 spellSlotIndex(std::shared_ptr<SpellData> spell);

private:
    std::array<std::shared_ptr<SpellData>, 4> spellSlots_;
    std::vector<std::shared_ptr<SpellData>> spells_;
    std::vector<std::string> spellFiles_;
    std::shared_ptr<SpellData> castedSpell_;
    std::unordered_map<u32, f32> cooldowns_;
    f32 castProgress_{0.0f};
    f32 castDuration_{0.0f};
};