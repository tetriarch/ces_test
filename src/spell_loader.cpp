#include "spell_loader.hpp"
#include "file_io.hpp"

#include <magic_enum/magic_enum.hpp>



std::string SpellLoader::getError(SpellLoaderError error) {
    if(error == SpellLoaderError::LOAD) {
        return "[SPELL LAODER]: failed to load spells\n" + errorMessage_.str();
    }
    if(error == SpellLoaderError::PARSE) {
        return "[SPELL LOADER]: failed to parse spells\n" + errorMessage_.str();
    }
    return "";
}

auto SpellLoader::load(const std::string& fileName) -> std::expected<std::vector<Spell>, SpellLoaderError> {

    // load source
    auto spellsSource = FileIO::readTextFile(fileName);
    if(!spellsSource) {
        addError("", fileName + " - " + spellsSource.error().message());
        return std::unexpected(SpellLoaderError::LOAD);
    }

    auto spells = parse(spellsSource.value());
    if(!spells) {
        return std::unexpected(spells.error());
    }

    return std::move(spells);
}

void SpellLoader::addError(const std::string& layer, const std::string& message) {
    if(!layer.empty()) {
        errorMessage_ << "[SPELL LOADER]: " << "<" << layer << ">: " << message << '\n';
    }
    else {
        errorMessage_ << "[SPELL LOADER]: " << message << '\n';
    }
}

auto SpellLoader::parse(const std::string& source) -> std::expected<std::vector<Spell>, SpellLoaderError> {

    json spellData;
    try {
        spellData = json::parse(source);
    }
    catch(json::exception& e) {
        addError("", e.what());
        return std::unexpected(SpellLoaderError::PARSE);
    }

    std::vector<Spell> spells;
    for(auto& [spell, spellDescription] : spellData.items()) {
        auto s = parseBasicStats(spellDescription, spell);
        if(!s) {
            addError(spell, "failed to parse spell basic stats");
            return std::unexpected(s.error());
        }

        json::const_iterator it = spellDescription.find("action");
        if(it == spellDescription.end()) {
            addError(spell, "has no action");
            return std::unexpected(SpellLoaderError::PARSE);
        }

        for(auto& action : it.value()) {
            auto a = parseAction(action, "action");
            if(!a) {
                addError(spell, "failed to parse action");
                return std::unexpected(a.error());
            }
            s->addAction(a.value());
        }
        spells.emplace_back(s.value());
    }
    return std::move(spells);
}

auto SpellLoader::parseBasicStats(const json& spellData, const std::string& parent) -> std::expected<Spell, SpellLoaderError> {

    std::string name;
    f32 castTime;
    f32 interruptTime;
    s32 manaCost;
    f32 cooldown;

    if(!get<std::string>(spellData, "name", true, name, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }
    if(!get<f32>(spellData, "cast_time", true, castTime, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<f32>(spellData, "interrupt_time", true, interruptTime, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<s32>(spellData, "mana_cost", true, manaCost, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<f32>(spellData, "cooldown", true, cooldown, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    return Spell(name, castTime, interruptTime, manaCost, cooldown);
}

auto SpellLoader::parseAction(const json& actionData, const std::string& parent) -> std::expected<SpellAction, SpellLoaderError> {

    SpellAction action;

    std::string actionType;
    bool pierce;
    std::string movementType;

    if(!get<std::string>(actionData, "type", true, actionType, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<bool>(actionData, "pierce", true, pierce, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    action.type = magic_enum::enum_cast<ActionType>(actionType).value_or(ActionType::UNKNOWN);
    action.pierce = pierce;

    // movement
    json::const_iterator it = actionData.find("movement");
    if(it == actionData.end()) {
        addError(parent, "has no movement");
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<std::string>(it.value(), "type", true, movementType, "movement")) {
        return std::unexpected(SpellLoaderError::PARSE);
    }
    if(movementType == "constant_motion") {
        ConstantMotion motion;
        f32 speed;
        if(!get<f32>(it.value(), "speed", true, speed, parent)) {
            return std::unexpected(SpellLoaderError::PARSE);
        }
        action.motion = std::make_shared<ConstantMotion>(motion);
    }
    else {
        InstantMotion motion;
        action.motion = std::make_shared<InstantMotion>(motion);
    }

    // on hit
    it = actionData.find("on_hit");
    if(it == actionData.end()) {
        addError(parent, "has no on_hit");
        return std::unexpected(SpellLoaderError::PARSE);
    }

    for(auto& oh : it.value()) {
        auto onHitAction = parseOnHitAction(oh, "on_hit");
        if(!onHitAction) {
            addError(parent, "failed to parse on_hit action");
            return std::unexpected(SpellLoaderError::PARSE);
        }
        action.actions.emplace_back(onHitAction.value());
    }

    return action;
}

auto SpellLoader::parseOnHitAction(const json& onHitData, const std::string& parent) -> std::expected<OnHitAction, SpellLoaderError> {

    std::unordered_map<std::string, std::string> effectMap;
    effectMap.emplace("DAMAGE", "damage_range");
    effectMap.emplace("DOT", "dot");
    effectMap.emplace("SLOW", "slow");
    effectMap.emplace("STUN", "stun");

    OnHitAction onHitAction;
    std::string effect;
    std::string damageType;

    if(!get<std::string>(onHitData, "effect", true, effect, "on_hit")) {
        return std::unexpected(SpellLoaderError::PARSE);
    }
    if(!get<std::string>(onHitData, "type", true, damageType, "on_hit")) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    onHitAction.type = magic_enum::enum_cast<EffectType>(effect).value_or(EffectType::UNKNOWN);
    onHitAction.damageType = magic_enum::enum_cast<DamageType>(damageType).value_or(DamageType::UNKNOWN);

    // make sure coresponding key based on effect type is found
    if(auto em = effectMap.find(effect); em != effectMap.end()) {
        json::const_iterator it = onHitData.find(em->second);
        if(it == onHitData.end()) {
            addError(parent, it.key() + " not found");
            return std::unexpected(SpellLoaderError::PARSE);
        }

        // individual effects - honestly this should be broken to functions
        if(it.key() == "damage_range") {
            Damage dmgRange;
            if(!get<s32>(it.value(), "min", true, dmgRange.damageRange.min, "damage_range")) {
                return std::unexpected(SpellLoaderError::PARSE);
            }
            if(!get<s32>(it.value(), "max", true, dmgRange.damageRange.max, "damage_range")) {
                return std::unexpected(SpellLoaderError::PARSE);
            }
            onHitAction.effect.damage.damageRange = dmgRange.damageRange;
        }

        if(it.key() == "dot") {
            Damage dot;
            if(!get<s32>(it.value(), "periodic_damage", true, dot.damageOverTime.periodicDamage, "dot")) {
                return std::unexpected(SpellLoaderError::PARSE);
            }
            if(!get<f32>(it.value(), "duration_in_sec", true, dot.damageOverTime.duration, "dot")) {
                return std::unexpected(SpellLoaderError::PARSE);
            }
            onHitAction.effect.damage.damageOverTime = dot.damageOverTime;
        }
        if(it.key() == "slow") {
            Debuff debuff;
            if(!get<s32>(it.value(), "magnitude", true, debuff.slow.magnitude, "slow")) {
                return std::unexpected(SpellLoaderError::PARSE);
            }
            if(!get<f32>(it.value(), "duration_in_sec", true, debuff.slow.duration, "slow")) {
                return std::unexpected(SpellLoaderError::PARSE);
            }
            onHitAction.effect.debuff.slow = debuff.slow;
        }
        if(it.key() == "stun") {
            Debuff debuff;
            if(!get<f32>(it.value(), "duration_in_sec", true, debuff.stun.duration, "stun")) {
                return std::unexpected(SpellLoaderError::PARSE);
            }
            onHitAction.effect.debuff.stun = debuff.stun;
        }

    }
    else {
        addError(parent, "associated data to effect type not found");
        return std::unexpected(SpellLoaderError::PARSE);
    }

    return onHitAction;
}

auto SpellLoader::valueTypeToTypeID(const json& value) -> std::type_index {

    if(value.is_null()) return typeid(nullptr);
    if(value.is_string()) return typeid(std::string);
    if(value.is_boolean()) return typeid(bool);
    if(value.is_number_integer()) return typeid(s32);
    if(value.is_number_float()) return typeid(f32);

    return typeid(void);
}

template<typename T>
bool SpellLoader::get(const json& object, std::string key, bool required, T& result, const std::string& parent) {

    json::const_iterator it = object.find(key);
    if(it == object.end()) {
        if(required) {
            std::string error = key + " not found";
            if(!parent.empty()) {
                error += " in " + parent;
            }
            addError(parent, error);
        }
        return false;
    }

    if(valueTypeToTypeID(it.value()) != typeid(T)) {
        if(required) {
            std::string error = key + " is a " + it.value().type_name() + ", expected " + typeToString<T>();
            if(!parent.empty()) {
                error = error + " in " + parent;
            }
            addError(parent, error);
        }
        return false;
    }
    result = it->get<T>();
    return true;
}

template<typename T>
auto SpellLoader::typeToString()->std::string {
    if(typeid(T) == typeid(std::string)) return "string";
    if(typeid(T) == typeid(s32)) return "int";
    if(typeid(T) == typeid(u32)) return "unsigned int";
    if(typeid(T) == typeid(f32)) return "float";
    if(typeid(T) == typeid(bool)) return "bool";

    else {
        return "unknown";
    }
}

