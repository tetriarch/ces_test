#include "scene.hpp"

#include "entity_manager.hpp"

auto Scene::create(const std::string& name) -> std::shared_ptr<Scene> {

    auto scene = std::make_shared<Scene>(name);
    EntityManager::get().addEntity(scene);
    return scene;
}