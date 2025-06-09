#include "status_effect_loader.hpp"

#include "file_io.hpp"
#include "log.hpp"

auto StatusEffectLoader::load(AssetManager& assetManager, const std::string& filePath)
    -> IAssetPtr {
    auto statusEffectSource = FileIO::readTextFile(filePath);
    if(!statusEffectSource) {
        ERROR(error(filePath + " " + statusEffectSource.error().message()))
        return nullptr;
    }

    auto statusEffect = parseStatusEffect(statusEffectSource.value());
    if(!statusEffect) {
        ERROR(error(filePath + " failed to parse"));
        return nullptr;
    }

    return std::make_shared<StatusEffectData>(statusEffect.value());
}

auto StatusEffectLoader::parseStatusEffect(const std::string& source)
    -> std::expected<StatusEffectData, JSONParserError> {
    json statusEffectJSON;
    try {
        statusEffectJSON = json::parse(source);
    } catch(const json::exception& e) {
        ERROR(error(std::string(e.what())));
        return std::unexpected(JSONParserError::PARSE);
    }

    StatusEffectData sed;
    if(!get<std::string>(statusEffectJSON, "texture_path", true, sed.textureFilePath)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<bool>(statusEffectJSON, "animated", true, sed.animated)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    json::const_iterator it;
    if(sed.animated) {
        it = statusEffectJSON.find("animations");
        if(it == statusEffectJSON.end()) {
            ERROR(error("failed to find animations"));
            return std::unexpected(JSONParserError::PARSE);
        }
        auto animations = parseAnimations(it.value(), "animations");
        if(!animations) {
            ERROR(error("failed to parse animations"));
            return std::unexpected(JSONParserError::PARSE);
        }
        sed.animationFiles = std::move(animations.value());
    }

    it = statusEffectJSON.find("rect");
    if(it == statusEffectJSON.end()) {
        ERROR(error("failed to find animations"));
        return std::unexpected(JSONParserError::PARSE);
    }

    auto rect = parseRect(it.value(), "rect");
    if(!rect) {
        ERROR(error("failed to find animations"));
        return std::unexpected(JSONParserError::PARSE);
    }
    sed.rect = std::move(rect.value());

    return std::move(sed);
}

auto StatusEffectLoader::parseAnimations(const json& o, const std::string& parent)
    -> std::expected<std::unordered_map<std::string, std::string>, JSONParserError> {
    if(!o.is_object()) {
        ERROR(error(parent + " is not an object", parent));
        return std::unexpected(JSONParserError::PARSE);
    }
    std::unordered_map<std::string, std::string> animations;
    for(auto& [key, value] : o.items()) {
        if(!value.is_string()) {
            ERROR(error(key + " is not a string", parent));
            return std::unexpected(JSONParserError::PARSE);
        }
        animations.insert(std::pair<std::string, std::string>(key, value));
    }
    return animations;
}

auto StatusEffectLoader::parseRect(const json& o, const std::string& parent)
    -> std::expected<Rect, JSONParserError> {
    if(!o.is_object()) {
        ERROR(error(parent + " is not an object", parent));
        return std::unexpected(JSONParserError::PARSE);
    }

    Rect rect;
    if(!get<f32>(o, "x", true, rect.x, "rect")) {
        return std::unexpected(JSONParserError::PARSE);
    }
    if(!get<f32>(o, "y", true, rect.y, "rect")) {
        return std::unexpected(JSONParserError::PARSE);
    }
    if(!get<f32>(o, "w", true, rect.w, "rect")) {
        return std::unexpected(JSONParserError::PARSE);
    }
    if(!get<f32>(o, "h", true, rect.h, "rect")) {
        return std::unexpected(JSONParserError::PARSE);
    }
    return rect;
}

auto StatusEffectLoader::error(const std::string& msg, const std::string& parent) -> std::string {
    std::string error = "[STATUS EFFECT LOADER]: ";
    if(!parent.empty()) {
        error += '(' + parent + ") ";
    }
    return error + msg;
}
