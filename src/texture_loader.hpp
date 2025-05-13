#pragma once

#include "i_asset_loader.hpp"
#include "texture.hpp"

#include <SDL3/SDL.h>

class AssetManager;
class Renderer;

class TextureLoader : public IAssetLoader {

public:
    TextureLoader(std::shared_ptr<Renderer> renderer);

    auto load(AssetManager& assetManager, const std::string& assetPath) -> IAssetPtr;
private:
    std::weak_ptr<Renderer> renderer_;
};