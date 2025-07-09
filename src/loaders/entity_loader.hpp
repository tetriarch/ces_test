#pragma once

#include "../entity.hpp"
#include "../entity_data.hpp"
#include "i_asset_loader.hpp"
#include "json_parser.hpp"

class EntityLoader : public IAssetLoader {
public:
    auto load(AssetManager& assetManager, const std::string& filepath) -> IAssetPtr override;

private:
    auto parseEntityData(const std::string& source) -> std::expected<EntityData, JSONParserError>;
};
