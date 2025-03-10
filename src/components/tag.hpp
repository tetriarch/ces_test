#pragma once

#include "../component.hpp"

enum class TagType {
    NPC,
    PLAYER,
    ENEMY,
    UNKNOWN
};

class TagComponent : public Component<TagComponent> {

public:
    void setTag(TagType tag) { tag_ = tag; }
private:
    TagType tag_{TagType::NPC};
    std::string tagToString(TagType tag) {
        switch(tag) {
            case TagType::NPC: return "NPC";
            case TagType::PLAYER: return "PLAYER";
            case TagType::ENEMY: return "ENEMY";
            default: return "";
        }
    }
};