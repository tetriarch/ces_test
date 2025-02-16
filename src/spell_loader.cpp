#include "spell_loader.hpp"
#include "file_io.hpp"

#include <nlohmann/json.hpp>

using namespace nlohmann;

std::string SpellLoader::getError(SpellLoaderError error) {
    if(error == SpellLoaderError::LOAD) {
        return "[SPELL PARSER]: failed to load spells\n" + errorMessage_;
    }
    if(error == SpellLoaderError::PARSE) {
        return "[SPELL PARSER]: failed to parse spells\n" + errorMessage_;
    }
    return "";
}

auto SpellLoader::load(const std::string& fileName) -> std::expected<std::vector<Spell>, SpellLoaderError> {

    // load source
    auto spellsSource = FileIO::readTextFile(fileName);
    if(!spellsSource) {
        errorMessage_ = fileName + " - " + spellsSource.error().message();
        return std::unexpected(SpellLoaderError::LOAD);
    }

    auto spells = parse(spellsSource.value());
    if(!spells) {
        return std::unexpected(spells.error());
    }

    return std::move(spells);
}

auto SpellLoader::parse(const std::string& source) -> std::expected<std::vector<Spell>, SpellLoaderError> {

    json spellData;
    try {
        spellData = json::parse(source);
    }
    catch(json::exception& e) {
        errorMessage_ = e.what();
        return std::unexpected(SpellLoaderError::PARSE);
    }

    std::vector<Spell> spells;
    if(spellData.is_object()) {
        for(auto& spellDataIterator : spellData.items()) {
            auto& s = spellDataIterator.value();

            //NOTE: This is very naive way there should be checks for every field, array etc...
            std::string name;
            f32 castTime;
            f32 interruptTime;
            u32 manaCost;
            f32 cooldown;
            try {
                s.at("name").get_to(name);
                s.at("cast_time").get_to(castTime);
                s.at("interrupt_time").get_to(interruptTime);
                s.at("mana_cost").get_to(manaCost);
                s.at("cooldown").get_to(cooldown);
                for(auto& a : *s.find("action")) {
                    // OnHitAction action;
                    //TODO: Figure out how to get actual action type from string in json, unorderd_map might do it
                    //NOTE: reflection in Washu's sample seems to hold the key information to this
                    // a.at("type").get_to(action.type);
                }
            }
            catch(json::exception& e) {
                errorMessage_ = e.what();
                return std::unexpected(SpellLoaderError::PARSE);
            }
            spells.emplace_back(name, castTime, interruptTime, manaCost, cooldown);
        }
    }
    return std::move(spells);
}