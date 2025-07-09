#pragma once

#include "../asset_manager.hpp"
#include "../components/animation.hpp"
#include "json_parser.hpp"

class AnimationLoader : public IAssetLoader, JSONParser {
public:
    auto load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr override;

private:
    auto parseAnimation(const std::string& source) -> std::expected<AnimationData, JSONParserError>;
    auto parseAnimationData(const json& o, const std::string& parent = "")
        -> std::expected<AnimationData, JSONParserError>;
    auto error(const std::string& msg, const std::string& parent = "") const
        -> std::string override;
};
