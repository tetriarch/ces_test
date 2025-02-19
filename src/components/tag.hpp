#pragma once

#include "../component.hpp"

enum class TagType {
    NPC,
    PLAYER,
    ENEMY
};

class TagComponent : public Component<TagComponent> {

public:
    auto describe() -> std::string override { return "I am " + tagToString(tag_); }
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