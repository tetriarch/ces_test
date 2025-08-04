#pragma once
#include <functional>
#include <vector>

#include "utils.hpp"

enum class EntityState {
    Idle,
    Updating,
};

enum class StructureUpdateType { Deferred, Immediate };

struct EntityStructureModifier {
    static void addChild(
        const EntityPtr& parent, const EntityPtr& child,
        StructureUpdateType updateType = StructureUpdateType::Deferred);

    static void removeChild(
        const EntityPtr& parent, const EntityPtr& child,
        StructureUpdateType updateType = StructureUpdateType::Deferred);

    static void addComponent(
        const EntityPtr& parent, const ComponentPtr& component,
        StructureUpdateType updateType = StructureUpdateType::Deferred);

    static void removeComponent(
        const EntityPtr& parent, const ComponentPtr& component,
        StructureUpdateType updateType = StructureUpdateType::Deferred);

    static void beginUpdate();

    static void endUpdate();

    static void applyStructureModifications();

private:
    static EntityState state_;
    static std::vector<std::function<void()>> modifications_;
};
