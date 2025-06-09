#pragma once

#include "asset_manager.hpp"
#include "components/status_effect.hpp"
#include "json_parser.hpp"
#include "utils.hpp"

class StatusEffectLoader : public IAssetLoader, JSONParser {
public:
    auto load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr override;

private:
    auto parseStatusEffect(const std::string& source)
        -> std::expected<StatusEffectData, JSONParserError>;
    auto parseAnimations(const json& o, const std::string& parent)
        -> std::expected<std::unordered_map<std::string, std::string>, JSONParserError>;
    auto parseRect(const json& o, const std::string& parent)
        -> std::expected<Rect, JSONParserError>;
    auto error(const std::string& msg, const std::string& parent = "") -> std::string override;
};
