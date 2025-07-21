#include "entity_creator.hpp"

#include <magic_enum/magic_enum.hpp>

#include "components/ai.hpp"
#include "components/collision.hpp"
#include "components/geometry.hpp"
#include "components/life.hpp"
#include "components/mana.hpp"
#include "components/player_control.hpp"
#include "components/reward.hpp"
#include "components/spell_book.hpp"
#include "components/status_effect.hpp"
#include "components/tag.hpp"
#include "components/velocity.hpp"
#include "components/xp.hpp"

EntityCreator::EntityCreator() {
    registerComponent(
        "life", [this](const json& JSONData) { return parseLifeComponent(JSONData); });

    registerComponent(
        "mana", [this](const json& JSONData) { return parseManaComponent(JSONData); });

    registerComponent("tag", [this](const json& JSONData) { return parseTagComponent(JSONData); });

    registerComponent(
        "control", [this](const json& JSONData) { return parseControlComponent(JSONData); });

    registerComponent(
        "collision", [this](const json& JSONData) { return parseCollisionComponent(JSONData); });

    registerComponent(
        "spellbook", [this](const json& JSONData) { return parseSpellBookComponent(JSONData); });

    registerComponent(
        "velocity", [this](const json& JSONData) { return parseVelocityComponent(JSONData); });

    registerComponent(
        "geometry", [this](const json& JSONData) { return parseGeometryComponent(JSONData); });
    registerComponent("status_effect",
        [this](const json& JSONData) { return parseStatusEffectComponent(JSONData); });
    registerComponent("ai", [this](const json& JSONData) { return parseAIComponent(JSONData); });
    registerComponent("xp", [this](const json& JSONData) { return parseXPComponent(JSONData); });
    registerComponent(
        "reward", [this](const json& JSONData) { return parseRewardComponent(JSONData); });
}
EntityCreator::~EntityCreator() {
    componentParsers_.clear();
}

auto EntityCreator::createEntity(const std::string& name, std::shared_ptr<EntityData> source) const
    -> EntityPtr {
    // all initial entities in scene are lazy -> attached after they are all loaded
    auto entity = Entity::create(name, true /* lazy attach*/);
    auto components = parseComponents(source->data);
    if(!components) {
        ERROR(error("failed to parse components"));
        return nullptr;
    }

    for(auto& c : components.value()) {
        entity->addComponent(c);
    }

    return entity;
}

void EntityCreator::registerComponent(const std::string& type, ComponentParseMethod parser) {
    componentParsers_[type] = parser;
}

auto EntityCreator::parseComponents(const json& entityJSON) const
    -> std::expected<std::vector<ComponentPtr>, JSONParserError> {
    // get components array
    json::const_iterator componentsJSON = entityJSON.find("components");
    if(componentsJSON == entityJSON.end()) {
        ERROR(error("components not found", ""));
        return std::unexpected(JSONParserError::PARSE);
    }

    std::vector<ComponentPtr> components;

    // iterate through array and get type
    for(auto& c : componentsJSON.value()) {
        std::string type;
        if(!get<std::string>(c, "type", true, type, "components")) {
            return std::unexpected(JSONParserError::PARSE);
        }

        // look for designated component parser and parse the component
        auto it = componentParsers_.find(type);
        if(it == componentParsers_.end()) {
            ERROR("cannot find component parser for " + type);
            return std::unexpected(JSONParserError::PARSE);
        }

        auto component = it->second(c);
        if(!component) {
            ERROR(error("failed to parse " + type, "components"));
            return std::unexpected(JSONParserError::PARSE);
        }
        components.push_back(component);
    }
    return components;
}

auto EntityCreator::parseLifeComponent(const json& o) const -> ComponentPtr {
    Life life;
    if(!get<f32>(o, "current", true, life.current, "components")) {
        return nullptr;
    }
    if(!get<f32>(o, "max", true, life.max, "components")) {
        return nullptr;
    }
    if(!get<f32>(o, "regen", true, life.regen, "components")) {
        return nullptr;
    }
    LifeComponent lifeComponent;
    lifeComponent.setLife(life);
    return std::make_shared<LifeComponent>(lifeComponent);
}

auto EntityCreator::parseManaComponent(const json& o) const -> ComponentPtr {
    Mana mana;
    if(!get<f32>(o, "current", true, mana.current, "components")) {
        return nullptr;
    }
    if(!get<f32>(o, "max", true, mana.max, "components")) {
        return nullptr;
    }
    if(!get<f32>(o, "regen", true, mana.regen, "components")) {
        return nullptr;
    }
    ManaComponent manaComponent;
    manaComponent.setMana(mana);

    return std::make_shared<ManaComponent>(manaComponent);
}

auto EntityCreator::parseTagComponent(const json& o) const -> ComponentPtr {
    TagType tag;
    std::string tagString;
    std::vector<std::string> friends;
    std::vector<std::string> foes;

    if(!get<std::string>(o, "tag", true, tagString, "components")) {
        return nullptr;
    }

    if(!o.contains("factions")) {
        ERROR(error("failed to find factions", "components"));
        return nullptr;
    }

    auto factions = o["factions"];

    if(!factions.contains("friendly")) {
        ERROR(error("failed to find friendly in factions", "components"));
        return nullptr;
    }

    auto friendly = factions["friendly"];
    auto hostile = factions["hostile"];

    if(friendly.is_array()) {
        for(auto& f : friendly) {
            friends.push_back(f);
        }
    } else {
        ERROR(error("friendly is not an array", "components"));
        return nullptr;
    }

    if(hostile.is_array()) {
        for(auto& h : hostile) {
            foes.push_back(h);
        }
    } else {
        ERROR(error("hostile is not an array", "components"));
        return nullptr;
    }

    TagComponent tagComponent;

    tag = magic_enum::enum_cast<TagType>(tagString).value_or(TagType::UNKNOWN);
    tagComponent.setTag(tag);

    for(auto& f : friends) {
        tag = magic_enum::enum_cast<TagType>(f).value_or(TagType::UNKNOWN);
        tagComponent.associate(FactionType::FRIENDLY, tag);
    }

    for(auto& f : foes) {
        tag = magic_enum::enum_cast<TagType>(f).value_or(TagType::UNKNOWN);
        tagComponent.associate(FactionType::HOSTILE, tag);
    }

    return std::make_shared<TagComponent>(tagComponent);
}

auto EntityCreator::parseControlComponent(const json& o) const -> ComponentPtr {
    std::string controller;
    if(!get<std::string>(o, "controller", true, controller, "components")) {
        return nullptr;
    }

    if(controller == "player") {
        return std::make_shared<PlayerControlComponent>();
    }

    return nullptr;
}

auto EntityCreator::parseCollisionComponent(const json& o) const -> ComponentPtr {
    CollisionShape collisionShape;
    std::string shape;

    if(!get<std::string>(o, "shape", true, shape, "components")) {
        return nullptr;
    }

    if(shape == "rect") {
        json::const_iterator it = o.find(shape);
        if(it == o.end()) {
            ERROR(error("failed to find " + shape, "components"));
            return nullptr;
        }
        Rect rect;

        if(!get<f32>(it.value(), "x", true, rect.x, "components")) {
            return nullptr;
        }
        if(!get<f32>(it.value(), "y", true, rect.y, "components")) {
            return nullptr;
        }
        if(!get<f32>(it.value(), "w", true, rect.w, "components")) {
            return nullptr;
        }
        if(!get<f32>(it.value(), "h", true, rect.h, "components")) {
            return nullptr;
        }

        collisionShape = rect;
    }

    else if(shape == "circle") {
        json::const_iterator it = o.find(shape);
        if(it == o.end()) {
            ERROR(error("failed to find " + shape, "components"));
            return nullptr;
        }
        Circle circle;

        if(!get<f32>(it.value(), "x", true, circle.x, "components")) {
            return nullptr;
        }
        if(!get<f32>(it.value(), "y", true, circle.y, "components")) {
            return nullptr;
        }
        if(!get<f32>(it.value(), "r", true, circle.r, "components")) {
            return nullptr;
        }

        collisionShape = circle;
    }

    else if(shape == "line") {
        json::const_iterator it = o.find(shape);
        if(it == o.end()) {
            ERROR(error("failed to find " + shape, "components"));
            return nullptr;
        }
        Line line;

        if(!get<f32>(it.value(), "x1", true, line.p1.x, "components")) {
            return nullptr;
        }
        if(!get<f32>(it.value(), "y1", true, line.p1.y, "components")) {
            return nullptr;
        }
        if(!get<f32>(it.value(), "x2", true, line.p2.x, "components")) {
            return nullptr;
        }
        if(!get<f32>(it.value(), "y2", true, line.p2.y, "components")) {
            return nullptr;
        }

        collisionShape = line;
    } else {
        ERROR(error("invalid shape - " + shape));
        return nullptr;
    }

    auto comp = std::make_shared<CollisionComponent>();
    comp->setCollisionShape(collisionShape);
    return comp;
}

auto EntityCreator::parseSpellBookComponent(const json& o) const -> ComponentPtr {
    SpellBookComponent spellBookComponent;
    json::const_iterator spells = o.find("spells");
    if(spells == o.end()) {
        ERROR(error("spells not found", "components"));
        return nullptr;
    }

    if(!spells->is_array()) {
        ERROR(error("spells is not array", "components"));
        return nullptr;
    }

    for(auto& s : spells.value()) {
        spellBookComponent.addSpellFile(s);
    }
    return std::make_shared<SpellBookComponent>(spellBookComponent);
}

auto EntityCreator::parseVelocityComponent(const json& o) const -> ComponentPtr {
    f32 speed;
    if(!get<f32>(o, "speed", true, speed, "components")) {
        return nullptr;
    }

    VelocityComponent velocityComponent;
    velocityComponent.setSpeed(speed);
    return std::make_shared<VelocityComponent>(velocityComponent);
}

auto EntityCreator::parseGeometryComponent(const json& o) const -> ComponentPtr {
    std::string filePath;
    GeometryData geometryData;
    GeometryComponent geometryComponent;

    json::const_iterator it = o.find("rect");
    if(it == o.end()) {
        ERROR(error("rect not found", "components"));
        return nullptr;
    }

    json rectJSON = it.value();

    if(!get<f32>(rectJSON, "x", true, geometryData.rect.x, "components/rect")) {
        return nullptr;
    }

    if(!get<f32>(rectJSON, "y", true, geometryData.rect.y, "components/rect")) {
        return nullptr;
    }

    if(!get<f32>(rectJSON, "w", true, geometryData.rect.w, "components/rect")) {
        return nullptr;
    }

    if(!get<f32>(rectJSON, "h", true, geometryData.rect.h, "components/rect")) {
        return nullptr;
    }

    if(!get<std::string>(o, "texture_path", true, filePath, "components")) {
        return nullptr;
    }

    // assuming no sizeDeterminant for dynamic sizing of entities
    geometryData.sizeDeterminant = GeometrySizeDeterminant::NONE;

    geometryComponent.setGeometryData(geometryData);
    geometryComponent.setTextureFilePath(filePath);
    return std::make_shared<GeometryComponent>(geometryComponent);
}

auto EntityCreator::parseStatusEffectComponent(const json& o) const -> ComponentPtr {
    StatusEffectComponent statusEffectComponent;

    return std::make_shared<StatusEffectComponent>(statusEffectComponent);
}

auto EntityCreator::parseAIComponent(const json& o) const -> ComponentPtr {
    AIComponent aiComponent;
    f32 aggroRadius;

    if(!get<f32>(o, "aggro_radius", true, aggroRadius, "components")) {
        return nullptr;
    }
    aiComponent.setAggroRadius(aggroRadius);
    return std::make_shared<AIComponent>(aiComponent);
}

auto EntityCreator::parseXPComponent(const json& o) const -> ComponentPtr {
    XPComponent xpComponent;
    u32 level = 0;
    bool progression = false;
    if(!get<u32>(o, "level", true, level, "components")) {
        return nullptr;
    }
    if(get<bool>(o, "progression", false, progression, "components")) {
        xpComponent.setProgression(progression);
    }

    xpComponent.setLevel(level);
    return std::make_shared<XPComponent>(xpComponent);
}

auto EntityCreator::parseRewardComponent(const json& o) const -> ComponentPtr {
    RewardComponent rewardComponent;
    u32 xp = 0;
    if(!get<u32>(o, "xp", true, xp, "components")) {
        return nullptr;
    }
    rewardComponent.setXP(xp);
    return std::make_shared<RewardComponent>(rewardComponent);
}

auto EntityCreator::error(const std::string& msg, const std::string& parent) const -> std::string {
    std::string error = "[ENTITY CREATOR]: ";
    if(!parent.empty()) {
        error += '(' + parent + ") ";
    }
    return error + msg;
}
