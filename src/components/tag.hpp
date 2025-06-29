#pragma once

#include "../component.hpp"

enum class TagType { NPC, PLAYER, MONSTER, UNKNOWN };

enum class FactionType { FRIENDLY, HOSTILE, UNKNOWN };

class TagComponent : public Component<TagComponent> {
public:
    TagComponent();
    void setTag(TagType tag);
    void associate(FactionType faction, TagType tag);
    void disassociate(FactionType faction, TagType tag);
    bool isFriendly(TagType tag);
    bool isHostile(TagType tag);

    TagType tag();
    bool isTaggedAs(TagType tag);

private:
    TagType tag_;
    std::unordered_set<TagType> friends_;
    std::unordered_set<TagType> foes_;
    bool removeFromList(std::unordered_set<TagType>& list, TagType tag);
    auto tagToString(TagType tag) -> std::string;
};
