#pragma once

#include "entity.hpp"
#include "entity_creator.hpp"
#include "i_asset.hpp"

class Scene : public Entity, public IAsset {
public:
    static auto create(const std::string& name, bool lazyAttach) -> std::shared_ptr<Scene>;
    auto entityCreator() const -> const EntityCreator&;
    using Entity::Entity;

private:
    EntityCreator entityCreator_;
};
