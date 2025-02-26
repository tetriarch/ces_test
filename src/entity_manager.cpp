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
    entities_.emplace(entity->getID(), entity);
}



void EntityManager::removeEntity(EntityPtr entity) {
    if(!entity) {
        return;
    }
    entities_.erase(entity->getID());
}



auto EntityManager::find(u32 id) -> EntityPtr {
    if(auto it = entities_.find(id); it != entities_.end()) {
        auto e = it->second.lock();
        return e;
    }
    return nullptr;
}
