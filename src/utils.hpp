#pragma once

#include <cstdint>
#include <iostream>
#include <expected>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <typeindex>
#include <unordered_set>
#include <unordered_map>
#include <vector>

using f32 = float;
using s32 = int32_t;
using u32 = uint32_t;

class Entity;
class ComponentBase;
using EntityPtr = std::shared_ptr<Entity>;
using ComponentPtr = std::shared_ptr<ComponentBase>;