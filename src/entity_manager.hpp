#pragma once

#include "utils.hpp"

class EntityManager {

public:
    static EntityManager& get();
    ~EntityManager();

    void addEntity(EntityPtr entity);
    void removeEntity(EntityPtr entity);
    auto find(const u32 id) -> EntityPtr;
    auto entities() const->std::unordered_map<u32, EntityHandle> const&;

private:
    std::unordered_map<u32, EntityHandle> entities_;

private:
    EntityManager() = default;
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
    EntityManager(EntityManager&&) = delete;
    EntityManager& operator=(EntityManager&&) = delete;
};