#include "entity_manager.hpp"
#include "entity.hpp"

EntityManager& EntityManager::get() {
    static EntityManager instance;
    return instance;
}

EntityManager::~EntityManager() {
    entities_.clear();
}

void EntityManager::addEntity(EntityPtr entity) {

    assert(entity);
    entities_.emplace(entity->id(), entity);
}

void EntityManager::removeEntity(EntityPtr entity) {

    if(!entity) {
        return;
    }
    entities_.erase(entity->id());
}

auto EntityManager::find(const u32 id) -> EntityPtr {

    if(auto it = entities_.find(id); it != entities_.end()) {
        auto entity = it->second.lock();
        return entity;
    }
    return nullptr;
}

auto EntityManager::entities() const -> std::unordered_map<u32, EntityHandle> const& {
    return entities_;
}
