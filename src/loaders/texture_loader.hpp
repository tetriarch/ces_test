#pragma once

#include "../texture.hpp"
#include "i_asset_loader.hpp"

class AssetManager;
class Renderer;

class TextureLoader : public IAssetLoader {
public:
    TextureLoader(std::shared_ptr<Renderer> renderer);

    auto load(AssetManager& assetManager, const std::string& assetPath) -> IAssetPtr;

private:
    std::weak_ptr<Renderer> renderer_;
};
