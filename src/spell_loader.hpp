#pragma once

#include "asset_manager.hpp"
#include "components/spell.hpp"
#include "components/geometry.hpp"
#include "json_parser.hpp"
#include "math.hpp"

class SpellLoader : public IAssetLoader, JSONParser {

public:
    auto load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr override;

private:
    auto parseSpell(const std::string& source) -> std::expected<SpellData, JSONParserError>;
    auto parseBasicStats(const json& o, const std::string& parent = "") -> std::expected<SpellData, JSONParserError>;
    auto parseAction(const json& o, const std::string& parent = "") -> std::expected<SpellAction, JSONParserError>;
    auto parseOnHitEffect(const json& o, const std::string& parent = "") -> std::expected<SpellEffect, JSONParserError>;
    auto parseGeometryData(const json& o, const std::string& parent = "") -> std::expected<GeometryData, JSONParserError>;
    auto parseCollisionData(const json& o, const std::string& parent = "") -> std::expected<CollisionData, JSONParserError>;
    auto parseAnimations(const json& o, const std::string& parent = "") -> std::expected<std::unordered_map<std::string, std::string>, JSONParserError>;
    auto error(const std::string& msg, const std::string& parent = "") -> std::string override;
};