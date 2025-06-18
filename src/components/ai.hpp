#pragma once

#include "../component.hpp"
#include "../random_number_generator.hpp"

enum class AIMode { IDLE, PATROL, COMBAT };

struct AIState {
    AIMode mode{AIMode::IDLE};
};

class AIComponent : public Component<AIComponent> {
public:
    AIComponent();
    void attach() override;
    void update(const f32 dt) override;
    void setAggroRadius(f32 radius);

private:
    bool enemyInRange() const;

private:
    AIState state_;
    f32 idleTimer_;
    f32 aggroRadius_;
    mutable std::vector<EntityHandle> enemiesInRange_;
    EntityHandle target_;
    RandomNumberGenerator rng_;
};
