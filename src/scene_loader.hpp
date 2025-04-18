#pragma once

#include "i_asset_loader.hpp"
#include "json_parser.hpp"
#include "scene.hpp"

class SceneLoader : public IAssetLoader, JSONParser {

public:
    SceneLoader();
    auto load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr override;

private:
    using ComponentParseMethod = std::function<ComponentPtr(const json&)>;

    void registerComponent(const std::string& type, ComponentParseMethod parser);
    auto parseScene(AssetManager& assetManager, const std::string& source) -> std::expected<std::shared_ptr<Scene>, JSONParserError>;
    auto parseEntity(const std::string& source, const std::string& name) -> EntityPtr;
    auto parseComponents(const json& entityJSON) -> std::expected<std::vector<ComponentPtr>, JSONParserError>;
    auto parseLifeComponent(const json& o) -> ComponentPtr;
    auto parseManaComponent(const json& o) -> ComponentPtr;
    auto parseTagComponent(const json& o) -> ComponentPtr;
    auto parseControlComponent(const json& o) -> ComponentPtr;
    auto parseCollisionComponent(const json& o) -> ComponentPtr;
    auto parseSpellBookComponent(const json& o) -> ComponentPtr;
    auto parseVelocityComponent(const json& o) -> ComponentPtr;
    auto parseGeometryComponent(const json& o) -> ComponentPtr;
    auto parseStatusEffectComponent(const json& o) -> ComponentPtr;

    auto error(const std::string& msg, const std::string& parent = "") -> std::string override;

private:
    std::unordered_map<std::string, ComponentParseMethod> componentParsers_;
};