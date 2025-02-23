#pragma once

#include "utils.hpp"
#include "components/spell.hpp"

#include <nlohmann/json.hpp>

#include <sstream>

using namespace nlohmann;

enum class SpellLoaderError {
    LOAD,
    PARSE
};

class SpellLoader {

public:
    std::string getError(SpellLoaderError error);
    auto load(const std::string& fileName) -> std::expected<std::vector<Spell>, SpellLoaderError>;

private:
    void addError(const std::string& layer, const std::string& message);
    auto parseSpells(const std::string& source) -> std::expected<std::vector<Spell>, SpellLoaderError>;
    auto parseBasicStats(const json& spellData, const std::string& parent) -> std::expected<Spell, SpellLoaderError>;
    auto parseAction(const json& actionData, const std::string& parent) -> std::expected<SpellAction, SpellLoaderError>;
    auto parseOnHitEffect(const json& onHitData, const std::string& parent) -> std::expected<SpellEffectOnHit, SpellLoaderError>;

    template<typename T>
    bool get(const json& object, std::string key, bool required, T& result, const std::string& parent = "");

    template<typename T>
    auto typeToString() -> std::string;

    auto valueTypeToTypeID(const json& value) -> std::type_index;

private:
    std::stringstream errorMessage_;
};