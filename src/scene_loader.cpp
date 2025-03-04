#include "asset_manager.hpp"
#include "file_io.hpp"
#include "scene.hpp"
#include "scene_loader.hpp"


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

    auto scene = Scene::create(sceneName);

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
            ERROR(error("failed to parse entity"));
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
    auto entity = Entity::create(name);

    //TODO: parse components


    return entity;
}

auto SceneLoader::error(const std::string& msg, const std::string& parent) -> std::string {

    std::string error = "[SCENE LOADER]: ";
    if(!parent.empty()) {
        error += '(' + parent + ") ";
    }
    return error + msg;
}