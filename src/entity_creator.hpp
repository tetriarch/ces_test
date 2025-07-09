#pragma once

#include "entity_data.hpp"
#include "loaders/json_parser.hpp"
#include "utils.hpp"

class EntityCreator : public JSONParser {
public:
    EntityCreator();
    ~EntityCreator();
    auto createEntity(const std::string& name, std::shared_ptr<EntityData> source) const
        -> EntityPtr;

private:
    using ComponentParseMethod = std::function<ComponentPtr(const json&)>;
    void registerComponent(const std::string& type, ComponentParseMethod parser);
    auto parseComponents(const json& entityJSON) const
        -> std::expected<std::vector<ComponentPtr>, JSONParserError>;
    auto parseLifeComponent(const json& o) const -> ComponentPtr;
    auto parseManaComponent(const json& o) const -> ComponentPtr;
    auto parseTagComponent(const json& o) const -> ComponentPtr;
    auto parseControlComponent(const json& o) const -> ComponentPtr;
    auto parseCollisionComponent(const json& o) const -> ComponentPtr;
    auto parseSpellBookComponent(const json& o) const -> ComponentPtr;
    auto parseVelocityComponent(const json& o) const -> ComponentPtr;
    auto parseGeometryComponent(const json& o) const -> ComponentPtr;
    auto parseStatusEffectComponent(const json& o) const -> ComponentPtr;
    auto parseAIComponent(const json& o) const -> ComponentPtr;
    auto parseXPComponent(const json& o) const -> ComponentPtr;
    auto parseRewardComponent(const json& o) const -> ComponentPtr;
    auto error(const std::string& msg, const std::string& parent = "") const
        -> std::string override;

private:
    std::unordered_map<std::string, ComponentParseMethod> componentParsers_;
};
