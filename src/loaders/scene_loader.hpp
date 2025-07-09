#pragma once

#include "../scene.hpp"
#include "i_asset_loader.hpp"
#include "json_parser.hpp"

class SceneLoader : public IAssetLoader, JSONParser {
public:
    auto load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr override;

private:
    auto parseScene(AssetManager& assetManager, const std::string& source)
        -> std::expected<std::shared_ptr<Scene>, JSONParserError>;
    auto error(const std::string& msg, const std::string& parent = "") const
        -> std::string override;
};
