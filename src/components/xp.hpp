#include "../component.hpp"

class XPComponent : public Component<XPComponent> {
public:
    XPComponent();
    void gainXP(u32 amount);
    void gainXP(u32 sourceLevel, u32 amount);
    void setLevel(u32 level);
    void setProgression(bool progression);
    u32 level() const;
    u32 currentXP() const;
    u32 nextLevelXP() const;
    bool isMaxLevel() const;
    void update(const f32 dt) override;

private:
    void levelUp();
    void resetXP();

private:
    u32 currentXP_;
    u32 nextLevelXP_;
    u32 previousLevelXP_;
    u32 level_;
    bool progression_;
};
