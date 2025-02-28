#pragma once

#include "utils.hpp"

class AssetManager;
class IAssetLoader {

public:
    virtual std::shared_ptr<void> load(AssetManager& assetManager, const std::string& assetPath) = 0;
};