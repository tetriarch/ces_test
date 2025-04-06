#include "spell_loader.hpp"
#include "file_io.hpp"

#include <magic_enum/magic_enum.hpp>

auto SpellLoader::load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr {

    auto spellSource = FileIO::readTextFile((filePath));
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

auto SpellLoader::parseSpell(const std::string& source) -> std::expected<SpellData, JSONParserError> {

    json spellJSON;
    try {
        spellJSON = json::parse(source);
    }
    catch(const json::exception& e) {
        ERROR(error(std::string(e.what())));
        return std::unexpected(JSONParserError::PARSE);
    }

    // parse basic stats
    auto stats = parseBasicStats(spellJSON);
    if(!stats) {
        ERROR(error("failed to parse basic stats"));
        return std::unexpected(stats.error());
    }
    auto spell = stats.value();

    // parse actions
    json::const_iterator it = spellJSON.find("action");
    if(it == spellJSON.end()) {
        ERROR(error("failed to find action"));
        return std::unexpected(JSONParserError::PARSE);
    }

    for(auto& a : it.value()) {
        auto action = parseAction(a, "action");
        if(!action) {
            ERROR(error("failed to parse action"));
            return std::unexpected(action.error());
        }
        spell.actions.emplace_back(action.value());
    }

    // parse geometry
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

    return std::move(spell);
}

auto SpellLoader::parseBasicStats(const json& o, const std::string& parent) -> std::expected<SpellData, JSONParserError> {

    SpellData spellData;

    if(!get<std::string>(o, "name", true, spellData.name, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(o, "cast_time", true, spellData.castTime, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(o, "interrupt_time", true, spellData.interruptTime, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<u32>(o, "mana_cost", true, spellData.manaCost, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(o, "cooldown", true, spellData.cooldown, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(o, "duration", true, spellData.duration, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(o, "max_range", true, spellData.maxRange, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<std::string>(o, "texture_path", false, spellData.textureFilePath, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    return std::move(spellData);
}

auto SpellLoader::parseAction(const json& o, const std::string& parent) -> std::expected<SpellAction, JSONParserError> {

    SpellAction action;

    std::string actionType;
    bool pierce;
    std::string movementType;

    if(!get<std::string>(o, "type", true, actionType, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<bool>(o, "pierce", true, pierce, parent)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    action.type = magic_enum::enum_cast<ActionType>(actionType).value_or(ActionType::UNKNOWN);
    action.pierce = pierce;

    // movement
    json::const_iterator it = o.find("movement");
    if(it == o.end()) {
        ERROR(error("has no movement", parent));
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<std::string>(it.value(), "type", true, movementType, "movement")) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(movementType == "constant_motion") {
        ConstantMotion motion;
        if(!get<f32>(it.value(), "speed", true, motion.speed, parent)) {
            return std::unexpected(JSONParserError::PARSE);
        }
        action.motion = std::make_shared<ConstantMotion>(motion);
    }
    else if(movementType == "instant") {
        InstantMotion motion;
        action.motion = std::make_shared<InstantMotion>(motion);
    }
    else {
        ERROR(error("invalid movement type", "movement"));
        return std::unexpected(JSONParserError::PARSE);
    }

    // on hit
    it = o.find("on_hit");
    if(it == o.end()) {
        ERROR(error("has no on_hit", parent));
        return std::unexpected(JSONParserError::PARSE);
    }

    for(auto& oh : it.value()) {
        auto onHitEffect = parseOnHitEffect(oh, "on_hit");
        if(!onHitEffect) {
            ERROR(error("failed to parse on_hit action", parent));
            return std::unexpected(JSONParserError::PARSE);
        }
        action.effects.emplace_back(onHitEffect.value());
    }

    return action;
}

auto SpellLoader::parseOnHitEffect(const json& o, const std::string& parent) -> std::expected<SpellEffectOnHit, JSONParserError> {

    SpellEffectOnHit onHitEffect;
    std::string effectType;
    std::string damageType;

    if(!get<std::string>(o, "effect_type", true, effectType, "on_hit")) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<std::string>(o, "type", true, damageType, "on_hit")) {
        return std::unexpected(JSONParserError::PARSE);
    }

    onHitEffect.damageType = magic_enum::enum_cast<DamageType>(damageType).value_or(DamageType::UNKNOWN);

    json::const_iterator it = o.find(effectType);
    if(it == o.end()) {
        ERROR(error("has no " + effectType, parent));
        return std::unexpected(JSONParserError::PARSE);
    }

    // individual effects - honestly this should be broken to functions
    if(effectType == "direct_damage") {
        DirectDamage dd;
        if(!get<u32>(it.value(), "min", true, dd.min, "direct_damage")) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<u32>(it.value(), "max", true, dd.max, "direct_damage")) {
            return std::unexpected(JSONParserError::PARSE);
        }
        onHitEffect.effect = dd;
    }

    else if(effectType == "dot") {
        DamageOverTime dot;
        if(!get<u32>(it.value(), "periodic_damage", true, dot.periodicDamage, "dot")) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<f32>(it.value(), "duration_in_sec", true, dot.duration, "dot")) {
            return std::unexpected(JSONParserError::PARSE);
        }
        onHitEffect.effect = dot;
    }

    else if(effectType == "slow") {
        Slow slow;
        if(!get<u32>(it.value(), "magnitude", true, slow.magnitude, "slow")) {
            return std::unexpected(JSONParserError::PARSE);
        }
        if(!get<f32>(it.value(), "duration_in_sec", true, slow.duration, "slow")) {
            return std::unexpected(JSONParserError::PARSE);
        }
        onHitEffect.effect = slow;
    }

    else if(effectType == "stun") {
        Stun stun;
        if(!get<f32>(it.value(), "duration_in_sec", true, stun.duration, "stun")) {
            return std::unexpected(JSONParserError::PARSE);
        }
        onHitEffect.effect = stun;
    }

    else {
        ERROR(error("missing associated key to effect_type value: " + effectType, parent));
        return std::unexpected(JSONParserError::PARSE);
    }

    return onHitEffect;
}

auto SpellLoader::parseGeometryData(const json& o, const std::string& parent) -> std::expected<GeometryData, JSONParserError> {

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

auto SpellLoader::parseCollisionData(const json& o, const std::string& parent) -> std::expected<CollisionData, JSONParserError> {

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
    }
    else {
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

auto SpellLoader::error(const std::string& msg, const std::string& parent) -> std::string {

    std::string error = "[SPELL LOADER]: ";
    if(!parent.empty()) {
        error += '(' + parent + ") ";
    }
    return error + msg;
}
