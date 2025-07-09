#pragma once
#include "i_asset.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

struct EntityData : public IAsset {
    json data;
};
