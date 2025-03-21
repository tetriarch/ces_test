#include "asset_manager.hpp"
#include "file_io.hpp"
#include "math.hpp"
#include "scene.hpp"
#include "scene_loader.hpp"
#include "components/components.hpp"

#include <magic_enum/magic_enum.hpp>

SceneLoader::SceneLoader() {

    registerComponent("life", [this](const json& JSONData) {
        return parseLifeComponent(JSONData);
    });

    registerComponent("mana", [this](const json& JSONData) {
        return parseManaComponent(JSONData);
    });

    registerComponent("tag", [this](const json& JSONData) {
        return parseTagComponent(JSONData);
    });

    registerComponent("control", [this](const json& JSONData) {
        return parseControlComponent(JSONData);
    });

    registerComponent("collision", [this](const json& JSONData) {
        return parseCollisionComponent(JSONData);
    });

    registerComponent("spellbook", [this](const json& JSONData) {
        return parseSpellBookComponent(JSONData);
    });

    registerComponent("velocity", [this](const json& JSONData) {
        return parseVelocityComponent(JSONData);
    });

    registerComponent("geometry", [this](const json& JSONData) {
        return parseGeometryComponent(JSONData);
    });
}

auto SceneLoader::load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr {

    auto sceneSource = FileIO::readTextFile(filePath);
    if(!sceneSource) {
        ERROR(error(filePath + " " + sceneSource.error().message()));
        return nullptr;
    }

    auto scene = parseScene(assetManager, sceneSource.value());
    if(!scene) {
        ERROR(error(filePath + " failed to parse"));
        return nullptr;
    }

    return scene.value();

}

void SceneLoader::registerComponent(const std::string& type, ComponentParseMethod parser) {

    componentParsers_[type] = parser;
}

auto SceneLoader::parseScene(AssetManager& assetManager, const std::string& source)->std::expected<std::shared_ptr<Scene>, JSONParserError> {

    json sceneJSON;
    try {
        sceneJSON = json::parse(source);
    }
    catch(const json::exception& e) {
        ERROR(error(std::string(e.what())));
        return std::unexpected(JSONParserError::PARSE);
    }

    std::string sceneName;
    if(!get<std::string>(sceneJSON, "name", true, sceneName, "")) {
        return std::unexpected(JSONParserError::PARSE);
    }

    auto scene = Scene::create(sceneName, true /* lazyAttach */);

    // go through entities
    json::const_iterator entitiesJSON = sceneJSON.find("entities");
    if(entitiesJSON == sceneJSON.end()) {
        ERROR(error("has no entitites"));
        return std::unexpected(JSONParserError::PARSE);
    }

    for(auto& e : entitiesJSON.value()) {
        std::string entityName;
        std::string prefab;
        Transform transform;

        if(!get<std::string>(e, "name", true, entityName, "entities")) {
            return std::unexpected(JSONParserError::PARSE);
        }

        if(!get<std::string>(e, "prefab", true, prefab, "entities")) {
            return std::unexpected(JSONParserError::PARSE);
        }

        json transformJSON;
        if(!e.contains("transform")) {
            ERROR(error("has no trasform", "entitites"));
            return std::unexpected(JSONParserError::PARSE);
        }
        transformJSON = e["transform"];

        if(transformJSON.is_array() && transformJSON.size() == 3) {
            transform.position.x = transformJSON[0];
            transform.position.y = transformJSON[1];
            transform.rotationInDegrees = transformJSON[2];
        }
        else {
            ERROR(error("transform is invalid", "entitites"));
            return std::unexpected(JSONParserError::PARSE);

        }

        auto entitySource = FileIO::readTextFile(assetManager.getAssetPath(prefab).generic_string());
        if(!entitySource) {
            ERROR(error(prefab + " " + entitySource.error().message()));
            return nullptr;
        }

        auto entity = parseEntity(entitySource.value(), entityName);
        if(!entity) {
            ERROR(error("failed to parse entity " + prefab));
            return nullptr;
        }

        entity->setTransform(transform);

        scene->addChild(entity);
    }
    return scene;
}

auto SceneLoader::parseEntity(const std::string& source, const std::string& name) -> EntityPtr {

    json entityJSON;
    try {
        entityJSON = json::parse(source);
    }
    catch(const json::exception& e) {
        ERROR(error(std::string(e.what())));
        return nullptr;
    }

    //NOTE: all initial entities in scene are lazy -> attached after they are all loaded
    auto entity = Entity::create(name, true /* lazy attach*/);

    auto components = parseComponents(entityJSON);
    if(!components) {
        ERROR(error("failed to parse components"));
        return nullptr;
    }

    for(auto& c : components.value()) {
        entity->addComponent(c);
    }

    return entity;
}

auto SceneLoader::parseComponents(const json& entityJSON) -> std::expected<std::vector<ComponentPtr>, JSONParserError> {

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

auto SceneLoader::parseLifeComponent(const json& o) -> ComponentPtr {

    Life life;
    if(!get<u32>(o, "current", true, life.current, "components")) {
        return nullptr;
    }
    if(!get<u32>(o, "max", true, life.max, "components")) {
        return nullptr;
    }
    LifeComponent lifeComponent;
    lifeComponent.setLife(life);
    return std::make_shared<LifeComponent>(lifeComponent);
}

auto SceneLoader::parseManaComponent(const json& o) -> ComponentPtr {

    Mana mana;
    if(!get<u32>(o, "current", true, mana.current, "components")) {
        return nullptr;
    }
    if(!get<u32>(o, "max", true, mana.max, "components")) {
        return nullptr;
    }
    ManaComponent manaComponent;
    manaComponent.setMana(mana);

    return std::make_shared<ManaComponent>(manaComponent);
}

auto SceneLoader::parseTagComponent(const json& o) -> ComponentPtr {

    TagType tag;
    std::string to;
    if(!get<std::string>(o, "to", true, to, "components")) {
        return nullptr;
    }
    tag = magic_enum::enum_cast<TagType>(to).value_or(TagType::UNKNOWN);
    TagComponent tagComponent;
    tagComponent.setTag(tag);

    return std::make_shared<TagComponent>(tagComponent);
}

auto SceneLoader::parseControlComponent(const json& o) -> ComponentPtr {

    std::string controller;
    if(!get<std::string>(o, "controller", true, controller, "components")) {
        return nullptr;
    }

    if(controller == "player") {
        return std::make_shared<PlayerControlComponent>();
    }

    return nullptr;
}

auto SceneLoader::parseCollisionComponent(const json& o) -> ComponentPtr {

    Rect rect;
    rect.x = 0.0f;
    rect.y = 0.0f;

    json::const_iterator box = o.find("box");
    if(box == o.end()) {
        ERROR(error("failed to find box", "components"));
        return nullptr;
    }

    if(!get<f32>(box.value(), "width", true, rect.w, "components")) {
        return nullptr;
    }
    if(!get<f32>(box.value(), "height", true, rect.h, "components")) {
        return nullptr;
    }

    CollisionComponent collisionComponent;
    collisionComponent.setCollisionBox(rect);
    return std::make_shared<CollisionComponent>(collisionComponent);
}

auto SceneLoader::parseSpellBookComponent(const json& o) -> ComponentPtr {

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

auto SceneLoader::parseVelocityComponent(const json& o) -> ComponentPtr {

    f32 speed;
    if(!get<f32>(o, "speed", true, speed, "components")) {
        return nullptr;
    }

    VelocityComponent velocityComponent;
    velocityComponent.setSpeed(speed);
    return std::make_shared<VelocityComponent>(velocityComponent);
}

auto SceneLoader::parseGeometryComponent(const json& o) -> ComponentPtr {

    std::string filePath;
    Rect rect;
    GeometryComponent geometryComponent;

    json::const_iterator rectJSON = o.find("rect");
    if(rectJSON == o.end()) {
        ERROR(error("rect not found", "components"));
        return nullptr;
    }

    if(!get<f32>(rectJSON.value(), "x", true, rect.x, "components/rect")) {
        return nullptr;
    }

    if(!get<f32>(rectJSON.value(), "y", true, rect.y, "components/rect")) {
        return nullptr;
    }

    if(!get<f32>(rectJSON.value(), "w", true, rect.w, "components/rect")) {
        return nullptr;
    }

    if(!get<f32>(rectJSON.value(), "h", true, rect.h, "components/rect")) {
        return nullptr;
    }

    if(!get<std::string>(o, "texture_path", true, filePath, "components")) {
        return nullptr;
    }

    geometryComponent.setRect(rect);
    geometryComponent.setTextureFilePath(filePath);
    return std::make_shared<GeometryComponent>(geometryComponent);
}

auto SceneLoader::error(const std::string& msg, const std::string& parent) -> std::string {

    std::string error = "[SCENE LOADER]: ";
    if(!parent.empty()) {
        error += '(' + parent + ") ";
    }
    return error + msg;
}