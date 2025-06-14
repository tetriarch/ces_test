#pragma once

#include "../asset_manager.hpp"
#include "../components/particle_system.hpp"
#include "../utils.hpp"
#include "json_parser.hpp"

class ParticleLoader : public IAssetLoader, JSONParser {
public:
    auto load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr override;

private:
    auto parseParticle(const std::string& source) -> std::expected<ParticleData, JSONParserError>;
    auto error(const std::string& msg, const std::string& parent = "") -> std::string override;
};
