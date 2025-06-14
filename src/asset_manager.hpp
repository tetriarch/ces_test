#pragma once

#include "loaders/i_asset_loader.hpp"
#include "log.hpp"

class AssetManager {
public:
    static AssetManager* get();
    void setAssetRoot(const std::string& assetRoot);

public:
    template <class T>
    void registerLoader(std::shared_ptr<IAssetLoader> loader);

    template <class T>
    auto load(const std::string& assetPath) -> std::shared_ptr<T>;

    void unload(const std::string& assetPath);
    auto getAssetPath(const std::string& assetPath) const -> std::filesystem::path;

private:
    std::string assetRoot_;
    std::unordered_map<std::string, IAssetPtr> assets_;
    std::unordered_map<std::type_index, std::shared_ptr<IAssetLoader>> assetLoaders_;

private:
    AssetManager() = default;
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    AssetManager(AssetManager&&) = delete;
    AssetManager& operator=(const AssetManager&&) = delete;
};

template <class T>
inline void AssetManager::registerLoader(std::shared_ptr<IAssetLoader> loader) {
    if(assetLoaders_.contains(typeid(T))) {
        INFO("[ASSET MANAGER]: loader " + std::string(typeid(T).name()) + " is already registered");
        return;
    }
    assetLoaders_.emplace(typeid(T), std::move(loader));
}

template <class T>
inline auto AssetManager::load(const std::string& assetPath) -> std::shared_ptr<T> {
    // check if already cached
    if(auto it = assets_.find(assetPath); it != assets_.end()) {
        return std::static_pointer_cast<T>(it->second);
    }

    // find appropriate loader
    auto loaderIter = assetLoaders_.find(typeid(T));
    if(loaderIter == assetLoaders_.end()) {
        ERROR("[ASSET MANAGER]: loader " + std::string(typeid(T).name()) + " not found");
        return nullptr;
    }

    auto loader = loaderIter->second;

    // validate path
    auto path = getAssetPath(assetPath);
    if(!std::filesystem::exists(path)) {
        ERROR("[ASSET MANAGER] asset path " + path.generic_string() + " does not exist");
        return nullptr;
    }

    // load asset via loader
    auto asset = loader->load(*this, path.generic_string());

    // store asset
    assets_.emplace(assetPath, asset);

    return std::static_pointer_cast<T>(asset);
}
