#include "tag.hpp"
#include "../log.hpp"

void TagComponent::setTag(TagType tag) {
    tag_ = tag;
}

void TagComponent::associate(FactionType faction, TagType tag) {

    switch(faction) {
        case FactionType::FRIENDLY: {
            friends_.insert(tag);
            break;
        }
        case FactionType::HOSTILE: {
            foes_.insert(tag);
            break;
        }
        default: {
            ERROR("[TAG COMPONENT]: invalid faction type");
            break;
        }
    }
}

void TagComponent::disassociate(FactionType faction, TagType tag) {

    switch(faction) {
        case FactionType::FRIENDLY: {
            bool removed = removeFromList(friends_, tag);
            if(!removed) {
                INFO("[TAG COMPONENT]: " + tagToString(tag) + " not found in friends");
            }
            break;
        }
        case FactionType::HOSTILE: {
            bool removed = removeFromList(foes_, tag);
            if(!removed) {
                INFO("[TAG COMPONENT]: " + tagToString(tag) + " not found in foes");
            }
            break;
        }
        default:
            ERROR("[TAG COMPONENT]: invalid faction type");
            break;
    }
}

bool TagComponent::isFriendly(TagType tag) {

    if(auto it = friends_.find(tag); it != friends_.end()) {
        return true;
    }
    return false;
}

bool TagComponent::isHostile(TagType tag) {

    if(auto it = foes_.find(tag); it != foes_.end()) {
        return true;
    }
    return false;
}

TagType TagComponent::tag() {
    return tag_;
}

bool TagComponent::isTaggedAs(TagType tag) {
    return tag_ == tag;
}

bool TagComponent::removeFromList(std::unordered_set<TagType>& list, TagType tag) {

    auto it = list.find(tag);

    if(it == list.end()) {
        return false;
    }

    list.erase(it);
    return true;
}

auto TagComponent::tagToString(TagType tag) -> std::string {
    switch(tag) {
        case TagType::NPC: return "NPC";
        case TagType::PLAYER: return "PLAYER";
        case TagType::MONSTER: return "MONSTER";
        default: return "UNKNOWN";
    }
}