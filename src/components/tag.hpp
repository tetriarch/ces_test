#pragma once

#include "../component.hpp"

enum class Tag {
    NPC,
    PLAYER,
    ENEMY
};

class TagComponent : public Component<TagComponent> {

public:
    auto describe() -> std::string override { return "I am " + tagToString(tag_); }
    void setTag(Tag tag) { tag_ = tag; }
private:
    Tag tag_{Tag::NPC};
    std::string tagToString(Tag tag) {
        switch(tag) {
            case Tag::NPC: return "NPC";
            case Tag::PLAYER: return "PLAYER";
            case Tag::ENEMY: return "ENEMY";
            default: return "";
        }
    }
};