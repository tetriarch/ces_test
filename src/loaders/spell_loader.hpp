#pragma once

#include "../asset_manager.hpp"
#include "../components/geometry.hpp"
#include "../components/spell.hpp"
#include "json_parser.hpp"

class SpellLoader : public IAssetLoader, JSONParser {
public:
    auto load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr override;

private:
    auto parseSpell(const std::string& source) -> std::expected<SpellData, JSONParserError>;
    bool parseGeneralData(const json& o, SpellData& spell, const std::string& parent = "");
    bool parseAction(const json& o, SpellData& spell, const std::string& parent = "");
    auto parseOnHitEffect(const json& o, const std::string& parent = "")
        -> std::expected<SpellEffect, JSONParserError>;
    bool parseSpawnEffect(const json& o, SpellData& spell, const std::string& parent = "");
    auto parseSelfEffect(const json& o, const std::string& parent = "")
        -> std::expected<SpellEffect, JSONParserError>;
    auto parseGeometryData(const json& o, const std::string& parent = "")
        -> std::expected<GeometryData, JSONParserError>;
    auto parseCollisionData(const json& o, const std::string& parent = "")
        -> std::expected<CollisionData, JSONParserError>;
    auto parseAnimations(const json& o, const std::string& parent = "")
        -> std::expected<std::unordered_map<std::string, std::string>, JSONParserError>;
    auto parseEmitters(const json& o, const std::string& parent = "")
        -> std::expected<std::vector<std::string>, JSONParserError>;
    auto error(const std::string& msg, const std::string& parent = "") const
        -> std::string override;
};
