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
    entities_.emplace(entity->name(), entity);
}

void EntityManager::removeEntity(EntityPtr entity) {
    if(!entity) {
        return;
    }
    entities_.erase(entity->name());
}

auto EntityManager::find(const std::string& name) -> EntityPtr {
    if(auto it = entities_.find(name); it != entities_.end()) {
        auto e = it->second.lock();
        return e;
    }
    return nullptr;
}
