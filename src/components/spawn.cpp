#include "spawn.hpp"

#include "../asset_manager.hpp"
#include "../entity.hpp"
#include "../scene.hpp"

SpawnComponent::SpawnComponent() {
}

void SpawnComponent::attach() {
    if(spawnName_.empty()) {
        ERROR("[SPAWN]: spawnName is not provided");
        return;
    }
    if(spawnPrefabFile_.empty()) {
        ERROR("[SPAWN]: prefabFile is not provided");
        return;
    }
    auto am = AssetManager::get();
    entityData_ = am->load<EntityData>(spawnPrefabFile_);
}

void SpawnComponent::setSpawn(const std::string& name, const std::string& prefabFile) {
    spawnName_ = name;
    spawnPrefabFile_ = prefabFile;
}

void SpawnComponent::spawn(const Vec2& position) const {
    if(spawnName_.empty()) {
        ERROR("[SPAWN]: spawnName is empty");
        return;
    }

    auto ed = entityData_.lock();
    if(ed) {
        auto root = std::static_pointer_cast<Scene>(entity()->root());
        auto entity = root->entityCreator().createEntity(spawnName_, ed);
        Transform transform = {{0.0f, 0.0f}, 0.0f};
        transform.position = position;
        entity->setTransform(transform);
        root->addChild(entity);
    }
}
