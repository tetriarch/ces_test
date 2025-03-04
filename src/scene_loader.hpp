#pragma once

#include "i_asset_loader.hpp"
#include "json_parser.hpp"
#include "scene.hpp"

class SceneLoader : public IAssetLoader, JSONParser {

public:
    auto load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr override;

private:
    auto parseScene(AssetManager& assetManager, const std::string& source) -> std::expected<std::shared_ptr<Scene>, JSONParserError>;
    auto parseEntity(const std::string& source, const std::string& name) -> EntityPtr;
    auto error(const std::string& msg, const std::string& parent = "") -> std::string override;

};