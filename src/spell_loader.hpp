#pragma once

#include "utils.hpp"
#include "components/spell.hpp"


enum class SpellLoaderError {
    LOAD,
    PARSE
};

class SpellLoader {

public:
    std::string getError(SpellLoaderError error);
    auto load(const std::string& fileName) -> std::expected<std::vector<Spell>, SpellLoaderError>;

private:
    auto parse(const std::string& source) -> std::expected<std::vector<Spell>, SpellLoaderError>;
    std::string errorMessage_ = std::string();
};