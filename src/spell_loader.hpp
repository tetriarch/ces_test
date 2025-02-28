#pragma once

#include "asset_manager.hpp"
#include "utils.hpp"
#include "components/spell.hpp"

#include <nlohmann/json.hpp>

#include <sstream>

using namespace nlohmann;

enum class SpellLoaderError {
    PARSE
};

class SpellLoader : public IAssetLoader {

public:
    auto load(AssetManager& assetManager, const std::string& filePath) -> std::shared_ptr<void> override;

private:
    auto parseSpell(const std::string& source) -> std::expected<SpellData, SpellLoaderError>;
    auto parseBasicStats(const json& spellJSON, const std::string& parent = "") -> std::expected<SpellData, SpellLoaderError>;
    auto parseAction(const json& actionJSON, const std::string& parent = "") -> std::expected<SpellAction, SpellLoaderError>;
    auto parseOnHitEffect(const json& onHitJSON, const std::string& parent = "") -> std::expected<SpellEffectOnHit, SpellLoaderError>;

    template<typename T>
    bool get(const json& object, std::string key, bool required, T& result, const std::string& parent = "");

    template<typename T>
    auto typeToString() -> std::string;

    auto valueTypeToTypeID(const json& value) -> std::type_index;

    void error(const std::string& msg, const std::string& parent = "");
};