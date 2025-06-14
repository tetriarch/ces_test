#include "animation_loader.hpp"

#include "../file_io.hpp"

auto AnimationLoader::load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr {
    auto animationSource = FileIO::readTextFile(filePath);
    if(!animationSource) {
        ERROR(error(filePath + " " + animationSource.error().message()));
        return nullptr;
    }

    auto animation = parseAnimation(animationSource.value());
    if(!animation) {
        ERROR(error(filePath + " failed to parse"));
        return nullptr;
    }

    return std::make_shared<AnimationData>(animation.value());
}

auto AnimationLoader::parseAnimation(const std::string& source)
    -> std::expected<AnimationData, JSONParserError> {
    json animationJSON;

    try {
        animationJSON = json::parse(source);
    } catch(const json::exception& e) {
        ERROR(error(std::string(e.what())));
        return std::unexpected(JSONParserError::PARSE);
    }

    auto animation = parseAnimationData(animationJSON);
    if(!animation) {
        ERROR(error("failed to parse animation data"));
        return std::unexpected(animation.error());
    }

    return std::move(animation.value());
}

auto AnimationLoader::parseAnimationData(const json& o, const std::string& parent)
    -> std::expected<AnimationData, JSONParserError> {
    AnimationData animation;
    if(!get<std::string>(o, "name", true, animation.name)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<u32>(o, "index", true, animation.index)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<u32>(o, "frame_count", true, animation.frameCount)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(o, "frame_duration", true, animation.frameDuration)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<bool>(o, "looping", true, animation.looping)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    return std::move(animation);
}

auto AnimationLoader::error(const std::string& msg, const std::string& parent) -> std::string {
    std::string error = "[ANIMATION LOADER]: ";
    if(!parent.empty()) {
        error += '(' + parent + ") ";
    }
    return error + msg;
}
