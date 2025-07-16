#pragma once
#include "utils.hpp"

#include <functional>
#include <vector>

enum class EntityState {
    Idle,
    Updating,
};

enum class StructureUpdateType {
    Deferred,
    Immediate
};

struct EntityStructureModifier {
    static void addChild(
        EntityPtr parent,
        EntityPtr child,
        StructureUpdateType updateType = StructureUpdateType::Deferred);

    static void removeChild(
        EntityPtr parent,
        EntityPtr child,
        StructureUpdateType updateType = StructureUpdateType::Deferred);

    static void addComponent(
        EntityPtr parent,
        ComponentPtr component,
        StructureUpdateType updateType = StructureUpdateType::Deferred);

    static void removeComponent(
        EntityPtr parent,
        ComponentPtr component,
        StructureUpdateType updateType = StructureUpdateType::Deferred);

    static void beginUpdate();

    static void endUpdate();

    static void applyStructureModifications();

private:
    static EntityState state_;
    static std::vector<std::function<void()>> modifications_;
};
