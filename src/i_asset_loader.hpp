#pragma once

#include "utils.hpp"

class AssetManager;
class IAssetLoader {

public:
    virtual auto load(AssetManager& assetManager, const std::string& assetPath) -> IAssetPtr = 0;
};