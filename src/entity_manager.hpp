#pragma once

#include "utils.hpp"

class EntityManager {

public:
    static EntityManager& get();
    ~EntityManager();

    void addEntity(EntityPtr entity);
    void removeEntity(EntityPtr entity);
    auto find(const std::string& name) -> EntityPtr;

private:
    std::unordered_map<std::string, EntityHandle> entities_;

private:
    EntityManager() = default;
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
};