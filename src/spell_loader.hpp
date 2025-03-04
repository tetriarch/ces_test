#pragma once

#include "asset_manager.hpp"
#include "components/spell.hpp"
#include "json_parser.hpp"

class SpellLoader : public IAssetLoader, JSONParser {

public:
    auto load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr override;

private:
    auto parseSpell(const std::string& source) -> std::expected<SpellData, JSONParserError>;
    auto parseBasicStats(const json& spellJSON, const std::string& parent = "") -> std::expected<SpellData, JSONParserError>;
    auto parseAction(const json& actionJSON, const std::string& parent = "") -> std::expected<SpellAction, JSONParserError>;
    auto parseOnHitEffect(const json& onHitJSON, const std::string& parent = "") -> std::expected<SpellEffectOnHit, JSONParserError>;
    auto error(const std::string& msg, const std::string& parent = "") -> std::string override;
};