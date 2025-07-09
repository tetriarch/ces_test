#pragma once

#include "../component.hpp"
#include "../entity_data.hpp"
#include "../math.hpp"

class SpawnComponent : public Component<SpawnComponent> {
public:
    SpawnComponent();
    void attach() override;
    void setSpawn(const std::string& name, const std::string& prefabFile);
    void spawn(const Vec2& position) const;

private:
    std::string spawnName_;
    std::string spawnPrefabFile_;
    std::weak_ptr<EntityData> entityData_;
};
