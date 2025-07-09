#include "entity_loader.hpp"

#include "../file_io.hpp"

auto EntityLoader::load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr {
    auto eDataSource = FileIO::readTextFile(filePath);
    if(!eDataSource) {
        ERROR("[ENTITY LOADER]: " + eDataSource.error().message());
        return nullptr;
    }

    auto entityData = parseEntityData(eDataSource.value());
    if(!entityData) {
        ERROR("[ENTITY LOADER]: " + filePath + " failed to parse");
        return nullptr;
    }

    return std::make_shared<EntityData>(entityData.value());
}

auto EntityLoader::parseEntityData(const std::string& source)
    -> std::expected<EntityData, JSONParserError> {
    EntityData data;
    try {
        data.data = json::parse(source);
    } catch(const json::exception& e) {
        ERROR("[ENTITY LOADER]: " + std::string(e.what()));
        return std::unexpected(JSONParserError::PARSE);
    }
    return std::move(data);
}
