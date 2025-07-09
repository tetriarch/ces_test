#pragma once

#include "../asset_manager.hpp"
#include "../components/particle_system.hpp"
#include "json_parser.hpp"

class EmitterLoader : public IAssetLoader, JSONParser {
public:
    auto load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr override;

private:
    auto parseEmitter(const std::string& source) -> std::expected<EmitterData, JSONParserError>;
    auto error(const std::string& msg, const std::string& parent = "") const
        -> std::string override;
};
