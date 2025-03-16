#pragma once

#include "entity.hpp"
#include "i_asset.hpp"

class Scene : public Entity, public IAsset {

public:
    static auto create(const std::string& name, bool lazyAttach) -> std::shared_ptr<Scene>;
    using Entity::Entity;
};