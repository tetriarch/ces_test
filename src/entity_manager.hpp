#pragma once

#include "utils.hpp"

class EntityManager {

public:
    static EntityManager& get();
    ~EntityManager();

    void addEntity(EntityPtr entity);
    void removeEntity(EntityPtr entity);
    auto find(u32 id) -> EntityPtr;

private:
    std::unordered_map<u32, EntityHandle> entities_;

private:
    EntityManager() = default;
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
};