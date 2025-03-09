#pragma once

#include "i_asset_loader.hpp"
#include "texture.hpp"

#include <SDL3/SDL.h>

class AssetManager;

class TextureLoader : public IAssetLoader {

public:
    TextureLoader(SDL_Renderer* renderer);

    auto load(AssetManager& assetManager, const std::string& assetPath) -> IAssetPtr;
private:
    SDL_Renderer* renderer_;
};