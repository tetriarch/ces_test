#include "spell_loader.hpp"
#include "file_io.hpp"

#include <magic_enum/magic_enum.hpp>



auto SpellLoader::load(AssetManager& assetManager, const std::string& filePath) -> std::shared_ptr<void> {

    auto spellSource = FileIO::readTextFile((filePath));
    if(!spellSource) {
        ERROR("[SPELL LOADER]: " + filePath + " " + spellSource.error().message());
        return nullptr;
    }

    auto spell = parseSpell(spellSource.value());
    if(!spell) {
        ERROR("[SPELL LOADER]: " + filePath + " failed to parse");
        return nullptr;
    }

    return std::make_shared<SpellData>(spell.value());
}



auto SpellLoader::parseSpell(const std::string& source) -> std::expected<SpellData, SpellLoaderError> {

    json spellJSON;
    try {
        spellJSON = json::parse(source);
    }
    catch(const json::exception& e) {
        error(std::string(e.what()));
        return std::unexpected(SpellLoaderError::PARSE);
    }

    // parse basic stats
    auto stats = parseBasicStats(spellJSON);
    if(!stats) {
        error("failed to parse basic stats");
        return std::unexpected(stats.error());
    }
    auto spell = stats.value();

    // parse actions
    json::const_iterator it = spellJSON.find("action");
    if(it == spellJSON.end()) {
        error("failed to find action");
        return std::unexpected(SpellLoaderError::PARSE);
    }

    for(auto& a : it.value()) {
        auto action = parseAction(a, "action");
        if(!action) {
            error("failed to parse action");
            return std::unexpected(action.error());
        }
        spell.actions.emplace_back(action.value());
    }

    return std::move(spell);
}



auto SpellLoader::parseBasicStats(const json& spellJSON, const std::string& parent) -> std::expected<SpellData, SpellLoaderError> {

    SpellData spellData;

    if(!get<std::string>(spellJSON, "name", true, spellData.name, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<f32>(spellJSON, "cast_time", true, spellData.castTime, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<f32>(spellJSON, "interrupt_time", true, spellData.interruptTime, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<u32>(spellJSON, "mana_cost", true, spellData.manaCost, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<f32>(spellJSON, "cooldown", true, spellData.cooldown, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    return std::move(spellData);
}



auto SpellLoader::parseAction(const json& actionJSON, const std::string& parent) -> std::expected<SpellAction, SpellLoaderError> {

    SpellAction action;

    std::string actionType;
    bool pierce;
    std::string movementType;

    if(!get<std::string>(actionJSON, "type", true, actionType, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<bool>(actionJSON, "pierce", true, pierce, parent)) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    action.type = magic_enum::enum_cast<ActionType>(actionType).value_or(ActionType::UNKNOWN);
    action.pierce = pierce;

    // movement
    json::const_iterator it = actionJSON.find("movement");
    if(it == actionJSON.end()) {
        error("has no movement", parent);
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
    else if(movementType == "instant") {
        InstantMotion motion;
        action.motion = std::make_shared<InstantMotion>(motion);
    }
    else {
        error("invalid movement type", "movement");
        return std::unexpected(SpellLoaderError::PARSE);
    }

    // on hit
    it = actionJSON.find("on_hit");
    if(it == actionJSON.end()) {
        error("has no on_hit", parent);
        return std::unexpected(SpellLoaderError::PARSE);
    }

    for(auto& oh : it.value()) {
        auto onHitEffect = parseOnHitEffect(oh, "on_hit");
        if(!onHitEffect) {
            error("failed to parse on_hit action", parent);
            return std::unexpected(SpellLoaderError::PARSE);
        }
        action.actions.emplace_back(onHitEffect.value());
    }

    return action;
}



auto SpellLoader::parseOnHitEffect(const json& onHitJSON, const std::string& parent) -> std::expected<SpellEffectOnHit, SpellLoaderError> {

    SpellEffectOnHit onHitEffect;
    std::string effectType;
    std::string damageType;

    if(!get<std::string>(onHitJSON, "effect_type", true, effectType, "on_hit")) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    if(!get<std::string>(onHitJSON, "type", true, damageType, "on_hit")) {
        return std::unexpected(SpellLoaderError::PARSE);
    }

    onHitEffect.damageType = magic_enum::enum_cast<DamageType>(damageType).value_or(DamageType::UNKNOWN);

    json::const_iterator it = onHitJSON.find(effectType);
    if(it == onHitJSON.end()) {
        error("has no " + effectType, parent);
        return std::unexpected(SpellLoaderError::PARSE);
    }

    // individual effects - honestly this should be broken to functions
    if(effectType == "direct_damage") {
        DirectDamage dd;
        if(!get<u32>(it.value(), "min", true, dd.min, "direct_damage")) {
            return std::unexpected(SpellLoaderError::PARSE);
        }
        if(!get<u32>(it.value(), "max", true, dd.max, "direct_damage")) {
            return std::unexpected(SpellLoaderError::PARSE);
        }
        onHitEffect.effect = dd;
    }

    else if(effectType == "dot") {
        DamageOverTime dot;
        if(!get<u32>(it.value(), "periodic_damage", true, dot.periodicDamage, "dot")) {
            return std::unexpected(SpellLoaderError::PARSE);
        }
        if(!get<f32>(it.value(), "duration_in_sec", true, dot.duration, "dot")) {
            return std::unexpected(SpellLoaderError::PARSE);
        }
        onHitEffect.effect = dot;
    }

    else if(effectType == "slow") {
        Slow slow;
        if(!get<u32>(it.value(), "magnitude", true, slow.magnitude, "slow")) {
            return std::unexpected(SpellLoaderError::PARSE);
        }
        if(!get<f32>(it.value(), "duration_in_sec", true, slow.duration, "slow")) {
            return std::unexpected(SpellLoaderError::PARSE);
        }
        onHitEffect.effect = slow;
    }

    else if(effectType == "stun") {
        Stun stun;
        if(!get<f32>(it.value(), "duration_in_sec", true, stun.duration, "stun")) {
            return std::unexpected(SpellLoaderError::PARSE);
        }
        onHitEffect.effect = stun;
    }

    else {
        error("missing associated key to effect_type value: " + effectType, parent);
        return std::unexpected(SpellLoaderError::PARSE);
    }

    return onHitEffect;
}



auto SpellLoader::valueTypeToTypeID(const json& value) -> std::type_index {

    if(value.is_null()) return typeid(nullptr);
    if(value.is_string()) return typeid(std::string);
    if(value.is_boolean()) return typeid(bool);
    if(value.is_number_unsigned()) return typeid(u32);
    if(value.is_number_integer()) return typeid(s32);
    if(value.is_number_float()) return typeid(f32);

    return typeid(void);
}

void SpellLoader::error(const std::string& msg, const std::string& parent) {
    if(parent.empty()) {
        ERROR("[SPELL LOADER]: " + msg);
    }
    else {
        ERROR("[SPELL_LOADER]: " + '(' + parent + ") " + msg);
    }
}



template<typename T>
bool SpellLoader::get(const json& object, std::string key, bool required, T& result, const std::string& parent) {

    json::const_iterator it = object.find(key);
    if(it == object.end()) {
        if(required) {
            std::string err = key + " not found";
            if(!parent.empty()) {
                err += " in " + parent;
            }
            error(err, parent);
        }
        return false;
    }

    if(valueTypeToTypeID(it.value()) != typeid(T)) {
        if(required) {
            std::string err = key + " is a " + it.value().type_name() + ", expected " + typeToString<T>();
            if(!parent.empty()) {
                err = err + " in " + parent;
            }
            error(err, parent);
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