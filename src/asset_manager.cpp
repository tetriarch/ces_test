#include "asset_manager.hpp"
#include "log.hpp"

AssetManager* AssetManager::get() {
    static AssetManager instance;
    return &instance;
}

void AssetManager::setAssetRoot(const std::string& assetRoot) {
    assetRoot_ = assetRoot;
}

void AssetManager::unload(const std::string& assetPath) {

    if(auto it = assets_.find(assetPath); it != assets_.end()) {
        assets_.erase(it);
    }
}

auto AssetManager::getAssetPath(const std::string& assetPath) const -> std::filesystem::path {
    return std::filesystem::path(assetRoot_) / assetPath;
}

