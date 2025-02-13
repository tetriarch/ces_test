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
#include <vector>

using f32 = float;
using u32 = uint32_t;

class Entity;
class ComponentBase;
using EntityPtr = std::shared_ptr<Entity>;
using ComponentPtr = std::shared_ptr<ComponentBase>;