#include "spell_loader.hpp"

#include <magic_enum/magic_enum.hpp>

#include "../file_io.hpp"

auto SpellLoader::load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr {
    auto spellSource = FileIO::readTextFile(filePath);
    if(!spellSource) {
        ERROR(error(filePath + " " + spellSource.error().message()));
        return nullptr;
    }

    auto spell = parseSpell(spellSource.value());
    if(!spell) {
        ERROR(error(filePath + " failed to parse"));
        return nullptr;
    }

    return std::make_shared<SpellData>(spell.value());
}

auto SpellLoader::parseSpell(const std::string& source)
    -> std::expected<SpellData, JSONParserError> {
    json spellJSON;
    try {
        spellJSON = json::parse(source);
    } catch(const json::exception& e) {
        ERROR(error(std::string(e.what())));
        return std::unexpected(JSONParserError::PARSE);
    }

    SpellData spell;

    // parse action to know what this spell actually do
    json::const_iterator it = spellJSON.find("action");
    if(it == spellJSON.end()) {
        ERROR(error("failed to find action"));
        return std::unexpected(JSONParserError::PARSE);
    }

    auto action = parseAction(it.value(), spell, "action");
    if(!action) {
        ERROR(error("failed to parse action"));
        return std::unexpected(JSONParserError::PARSE);
    }

    // parse general stats
    auto stats = parseGeneralData(spellJSON, spell);
    if(!stats) {
        ERROR(error("failed to parse general stats"));
        return std::unexpected(JSONParserError::PARSE);
    }

    // parse geometry
    if(spell.requiresComponent(SpellRequirement::GEOMETRY)) {
        it = spellJSON.find("geometry");
        if(it == spellJSON.end()) {
            ERROR(error("failed to find geometry"));
            return std::unexpected(JSONParserError::PARSE);
        }

        auto geometry = parseGeometryData(it.value(), "geometry");
        if(!geometry) {
            ERROR(error("failed to parse geometry"));
            return std::unexpected(JSONParserError::PARSE);
        }

        spell.geometryData = std::move(geometry.value());
    }

    if(spell.requiresComponent(SpellRequirement::COLLISION)) {
        it = spellJSON.find("collision");
        if(it == spellJSON.end()) {
            ERROR(error("failed to find collision"));
            return std::unexpected(JSONParserError::PARSE);
        }

        auto collisionData = parseCollisionData(it.value(), "collision");
        if(!collisionData) {
            ERROR(error("failed to parse collision"));
            return std::unexpected(JSONParserError::PARSE);
        }
        spell.collisionData = std::move(collisionData.value());
    }

    // animation
    if(spell.requiresComponent(SpellRequirement::ANIMATION)) {
        it = spellJSON.find("animations");
        if(it == spellJSON.end()) {
            ERROR(error("failed to find animations"));
            return std::unexpected(JSONParserError::PARSE);
        }

        auto animations = parseAnimations(it.value(), "animations");
        if(!animations) {
            ERROR(error("failed to parse animations"));
            return std::unexpected(JSONParserError::PARSE);
        }
        spell.animationFiles = std::move(animations.value());
    }

    if(spell.requiresComponent(SpellRequirement::PARTICLE)) {
        it = spellJSON.find("emitters");
        if(it == spellJSON.end()) {
            ERROR(error("failed to find emitters"));
            return std::unexpected(JSONParserError::PARSE);
        }

        auto emitters = parseEmitters(it.value(), "emitters");
        if(!emitters) {
            ERROR(error("failed to parse animations"));
            return std::unexpected(JSONParserError::PARSE);
        }
        spell.emitterFiles = std::move(emitters.value());
    }

    return std::move(spell);
}

bool SpellLoader::parseGeneralData(const json& o, SpellData& spell, const std::string& parent) {
    if(!get<std::string>(o, "name", true, spell.name, parent)) {
        return false;
    }

    if(!get<f32>(o, "cast_time", true, spell.castTime, parent)) {
        return false;
    }

    if(!get<f32>(o, "interrupt_time", true, spell.interruptTime, parent)) {
        return false;
    }

    if(!get<u32>(o, "mana_cost", true, spell.manaCost, parent)) {
        return false;
    }

    if(!get<f32>(o, "cooldown", true, spell.cooldown, parent)) {
        return false;
    }

    if(!get<f32>(o, "duration", true, spell.duration, parent)) {
        return false;
    }

    if(!get<f32>(o, "max_range", true, spell.maxRange, parent)) {
        return false;
    }

    bool animated = false;
    if(!get<bool>(o, "animated", true, animated, parent)) {
        return false;
    }
    if(animated) {
        spell.componentRequirements |= static_cast<u8>(SpellRequirement::ANIMATION);
        spell.componentRequirements |= static_cast<u8>(SpellRequirement::GEOMETRY);
    }

    if(spell.requiresComponent(SpellRequirement::GEOMETRY)) {
        if(!get<std::string>(o, "texture_path", true, spell.textureFilePath, parent)) {
            return false;
        }
    }

    bool particles = false;
    if(!get<bool>(o, "particles", true, particles, parent)) {
        return false;
    }
    if(particles) {
        spell.componentRequirements |= static_cast<u8>(SpellRequirement::PARTICLE);
    }

    return true;
}

bool SpellLoader::parseAction(const json& o, SpellData& spell, const std::string& parent) {
    std::string actionType;
    std::string movementType;

    if(!get<std::string>(o, "type", true, actionType, parent)) {
        return false;
    }

    spell.action.type = magic_enum::enum_cast<ActionType>(actionType).value_or(ActionType::UNKNOWN);
    if(spell.action.type == ActionType::BEAM || spell.action.type == ActionType::PROJECTILE) {
        if(!get<bool>(o, "pierce", true, spell.action.pierce, parent)) {
            return false;
        }
    }

    // movement
    if(spell.action.type != ActionType::SELF && spell.action.type != ActionType::SPAWN &&
        spell.action.type != ActionType::UNKNOWN) {
        spell.componentRequirements |= static_cast<u8>(SpellRequirement::GEOMETRY);

        json::const_iterator it = o.find("movement");
        if(it == o.end()) {
            ERROR(error("has no movement", parent));
            return false;
        }

        if(!get<std::string>(it.value(), "type", true, movementType, "movement")) {
            return false;
        }

        if(movementType == "constant_motion") {
            ConstantMotion motion;
            if(!get<f32>(it.value(), "speed", true, motion.speed, parent)) {
                return false;
            }
            spell.action.motion = std::make_shared<ConstantMotion>(motion);
        } else if(movementType == "instant") {
            InstantMotion motion;
            spell.action.motion = std::make_shared<InstantMotion>(motion);
        } else {
            ERROR(error("invalid movement type", "movement"));
            return false;
        }
    }

    // on hit
    if(o.contains("on_hit")) {
        spell.componentRequirements |= static_cast<u8>(SpellRequirement::COLLISION);

        auto it = o.find("on_hit");
        if(it == o.end()) {
            ERROR(error("has no on_hit", parent));
            return false;
        }

        for(auto& oh : it.value()) {
            auto onHitEffect = parseOnHitEffect(oh, "on_hit");
            if(!onHitEffect) {
                ERROR(error("failed to parse on_hit action", parent));
                return false;
            }
            spell.action.effects.emplace_back(onHitEffect.value());
        }
    }

    // spawn
    if(o.contains("spawn")) {
        spell.componentRequirements |= static_cast<u8>(SpellRequirement::SPAWN);

        auto it = o.find("spawn");
        if(it == o.end()) {
            ERROR(error("has no spawn", parent));
            return false;
        }

        auto spawnEffect = parseSpawnEffect(it.value(), spell, "spawn");
        if(!spawnEffect) {
            ERROR(error("failed to parse spawn action", parent));
            return false;
        }
    }

    // self
    if(o.contains("self")) {
        auto it = o.find("self");
        if(it == o.end()) {
            ERROR(error("has no self", parent));
            return false;
        }

        for(auto& s : it.value()) {
            auto selfEffect = parseSelfEffect(s, "self");
            if(!selfEffect) {
                ERROR(error("failed to parse self action", parent));
                return false;
            }
            spell.action.effects.emplace_back(selfEffect.value());
        }
    }

    return true;
}

auto SpellLoader::parseOnHitEffect(const json& o, const std::string& parent)
    -> std::expected<SpellEffect, JSONParserError> {
    SpellEffect onHitEffect;
    std::string effectType;
    std::string dmgType;
    std::string targetFaction;

    // mandatory
    if(!get<std::string>(o, "name", true, onHitEffect.name, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<std::string>(o, "effect_type", true, effectType, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }
    onHitEffect.type =
        magic_enum::enum_cast<SpellEffectType>(effectType).value_or(SpellEffectType::UNKNOWN);

    if(!get<f32>(o, "duration", true, onHitEffect.maxDuration, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<std::string>(o, "damage_type", true, dmgType, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }
    onHitEffect.dmgType = magic_enum::enum_cast<DamageType>(dmgType).value_or(DamageType::UNKNOWN);

    if(!get<std::string>(o, "target_faction", true, targetFaction, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }
    onHitEffect.targetFaction =
        magic_enum::enum_cast<FactionType>(targetFaction).value_or(FactionType::UNKNOWN);

    if(!get<bool>(o, "visual", true, onHitEffect.visual, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(onHitEffect.visual) {
        if(!get<std::string>(o, "effect_path", true, onHitEffect.effectFilePath, parent)) {
            return std::unexpected(JSONParserError::PARSE);
        }
    }
    // optional
    get<u32>(o, "min_value", false, onHitEffect.minValue, parent);
    get<u32>(o, "max_value", false, onHitEffect.maxValue, parent);
    get<u32>(o, "periodic_value", false, onHitEffect.periodicValue, parent);
    get<f32>(o, "magnitude", false, onHitEffect.magnitude, parent);
    get<u32>(o, "max_stacks", false, onHitEffect.maxStacks, parent);

    return onHitEffect;
}

bool SpellLoader::parseSpawnEffect(const json& o, SpellData& spell, const std::string& parent) {
    if(!get<std::string>(o, "name", true, spell.spawnName, parent)) {
        return false;
    }
    if(!get<std::string>(o, "prefab", true, spell.spawnPrefabFile, parent)) {
        return false;
    }
    // for consistency
    return true;
}

auto SpellLoader::parseSelfEffect(const json& o, const std::string& parent)
    -> std::expected<SpellEffect, JSONParserError> {
    SpellEffect selfEffect;
    std::string effectType;

    // mandatory
    if(!get<std::string>(o, "name", true, selfEffect.name, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<std::string>(o, "effect_type", true, effectType, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }
    selfEffect.type =
        magic_enum::enum_cast<SpellEffectType>(effectType).value_or(SpellEffectType::UNKNOWN);

    if(!get<f32>(o, "duration", true, selfEffect.maxDuration, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<bool>(o, "visual", true, selfEffect.visual, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(selfEffect.visual) {
        if(!get<std::string>(o, "effect_path", true, selfEffect.effectFilePath, parent)) {
            return std::unexpected(JSONParserError::PARSE);
        }
    }
    // optional
    get<u32>(o, "min_value", false, selfEffect.minValue, parent);
    get<u32>(o, "max_value", false, selfEffect.maxValue, parent);
    get<u32>(o, "periodic_value", false, selfEffect.periodicValue, parent);
    get<f32>(o, "magnitude", false, selfEffect.magnitude, parent);
    get<u32>(o, "max_stacks", false, selfEffect.maxStacks, parent);

    return selfEffect;
}

auto SpellLoader::parseGeometryData(const json& o, const std::string& parent)
    -> std::expected<GeometryData, JSONParserError> {
    GeometryData geometryData;
    std::string sizeDeterminant;

    json::const_iterator it = o.find("rect");
    if(it == o.end()) {
        ERROR(error("has no rect", parent));
        return std::unexpected(JSONParserError::PARSE);
    }

    json rectJSON = it.value();

    if(!get<f32>(rectJSON, "x", true, geometryData.rect.x, "rect")) {
        return std::unexpected(JSONParserError::PARSE);
    }
    if(!get<f32>(rectJSON, "y", true, geometryData.rect.y, "rect")) {
        return std::unexpected(JSONParserError::PARSE);
    }
    if(!get<f32>(rectJSON, "w", true, geometryData.rect.w, "rect")) {
        return std::unexpected(JSONParserError::PARSE);
    }
    if(!get<f32>(rectJSON, "h", true, geometryData.rect.h, "rect")) {
        return std::unexpected(JSONParserError::PARSE);
    }
    if(!get<std::string>(o, "determinant", true, sizeDeterminant, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    geometryData.sizeDeterminant = magic_enum::enum_cast<GeometrySizeDeterminant>(sizeDeterminant)
                                       .value_or(GeometrySizeDeterminant::NONE);

    return geometryData;
}

auto SpellLoader::parseCollisionData(const json& o, const std::string& parent)
    -> std::expected<CollisionData, JSONParserError> {
    CollisionData collisionData;
    std::string shape;
    std::string sizeDeterminant;

    if(!get<std::string>(o, "shape", true, shape, "collision")) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(shape == "rect") {
        json::const_iterator it = o.find(shape);
        if(it == o.end()) {
            ERROR(error("failed to find " + shape, "collision"));
            return std::unexpected(JSONParserError::PARSE);
        }
        Rect rect;

        if(!get<f32>(it.value(), "x", true, rect.x, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<f32>(it.value(), "y", true, rect.y, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<f32>(it.value(), "w", true, rect.w, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<f32>(it.value(), "h", true, rect.h, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }

        collisionData.shape = rect;
    }

    else if(shape == "circle") {
        json::const_iterator it = o.find(shape);
        if(it == o.end()) {
            ERROR(error("failed to find " + shape, "collision"));
            return std::unexpected(JSONParserError::PARSE);
        }
        Circle circle;

        if(!get<f32>(it.value(), "x", true, circle.x, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<f32>(it.value(), "y", true, circle.y, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<f32>(it.value(), "r", true, circle.r, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }

        collisionData.shape = circle;
    }

    else if(shape == "line") {
        json::const_iterator it = o.find(shape);
        if(it == o.end()) {
            ERROR(error("failed to find " + shape, "collision"));
            return std::unexpected(JSONParserError::PARSE);
        }
        Line line;

        if(!get<f32>(it.value(), "x1", true, line.p1.x, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<f32>(it.value(), "y1", true, line.p1.y, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<f32>(it.value(), "x2", true, line.p2.x, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<f32>(it.value(), "y2", true, line.p2.y, shape)) {
            return std::unexpected(JSONParserError::PARSE);
        }

        collisionData.shape = line;
    } else {
        ERROR(error("invalid shape - " + shape));
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<std::string>(o, "determinant", true, sizeDeterminant, "")) {
        return std::unexpected(JSONParserError::PARSE);
    }

    collisionData.sizeDeterminant = magic_enum::enum_cast<CollisionSizeDeterminant>(sizeDeterminant)
                                        .value_or(CollisionSizeDeterminant::NONE);

    return collisionData;
}

auto SpellLoader::parseAnimations(const json& o, const std::string& parent)
    -> std::expected<std::unordered_map<std::string, std::string>, JSONParserError> {
    if(!o.is_object()) {
        ERROR(error(parent + " is not an object", parent));
        return std::unexpected(JSONParserError::PARSE);
    }

    std::unordered_map<std::string, std::string> animations;
    for(auto& [key, value] : o.items()) {
        if(!value.is_string()) {
            ERROR(error(key + " is not a string", parent));
            return std::unexpected(JSONParserError::PARSE);
        }
        animations.insert(std::pair<std::string, std::string>(key, value));
    }

    return animations;
}
auto SpellLoader::parseEmitters(const json& o, const std::string& parent)
    -> std::expected<std::vector<std::string>, JSONParserError> {
    if(!o.is_array()) {
        ERROR(error(parent + " is not an array", parent));
        return std::unexpected(JSONParserError::PARSE);
    }

    std::vector<std::string> emitters;
    for(auto& e : o) {
        if(!e.is_string()) {
            ERROR_ONCE(error(parent + " is not an array of strings", parent));
            return std::unexpected(JSONParserError::PARSE);
        }
        emitters.push_back(e);
    }
    return emitters;
}

auto SpellLoader::error(const std::string& msg, const std::string& parent) const -> std::string {
    std::string error = "[SPELL LOADER]: ";
    if(!parent.empty()) {
        error += '(' + parent + ") ";
    }
    return error + msg;
}
