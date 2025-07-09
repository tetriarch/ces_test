#include "scene.hpp"

auto Scene::create(const std::string& name, bool lazyAttach) -> std::shared_ptr<Scene> {
    auto scene = std::make_shared<Scene>(name, lazyAttach);
    return scene;
}

auto Scene::entityCreator() const -> const EntityCreator& {
    return entityCreator_;
}
