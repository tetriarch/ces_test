#pragma once

#include "../component.hpp"
#include "../random_number_generator.hpp"

enum class AIMode { IDLE, PATROL, COMBAT };

class AIComponent : public Component<AIComponent> {
public:
    AIComponent();
    void attach() override;

    void update(const f32 dt) override;
    void render(std::shared_ptr<Renderer> renderer) override;
    void setAggroRadius(f32 radius);
    EntityHandle target();
    void setTarget(EntityPtr target);
    bool isInMode(AIMode mode);
    void resolveEffectApplier(EntityPtr applier);

private:
    void updateIdle(const f32 dt);
    void updateCombat(const f32 dt);
    // popuplates enemies and allies in range lists
    void collectEntitiesInRange();
    bool enemyInRange() const;
    bool allyInRange() const;
    bool canEnterCombat();
    void enterCombat(EntityPtr target = nullptr);
    void leaveCombat();

private:
    AIMode mode_{AIMode::IDLE};
    f32 idleTimer_;
    f32 aggroRadius_;
    f32 combatEntryCooldown_;
    std::vector<EntityHandle> enemiesInRange_;
    std::vector<EntityHandle> alliesInRange_;
    EntityHandle target_;
    RandomNumberGenerator rng_;
};
