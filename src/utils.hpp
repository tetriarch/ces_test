#pragma once

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <expected>
#include <filesystem>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <typeindex>
#include <unordered_set>
#include <unordered_map>
#include <variant>
#include <vector>

using u8 = uint8_t;
using f32 = float;
using s32 = int32_t;
using u32 = uint32_t;

class Entity;
class ComponentBase;
class IAsset;

using IAssetPtr = std::shared_ptr<IAsset>;
using EntityPtr = std::shared_ptr<Entity>;
using EntityHandle = std::weak_ptr<Entity>;
using ComponentPtr = std::shared_ptr<ComponentBase>;