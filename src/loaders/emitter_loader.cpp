#include "emitter_loader.hpp"

#include <magic_enum/magic_enum.hpp>

#include "../file_io.hpp"

auto EmitterLoader::load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr {
    auto emitterSource = FileIO::readTextFile(filePath);
    if(!emitterSource) {
        ERROR(error(filePath + " " + emitterSource.error().message()));
        return nullptr;
    }

    auto emitter = parseEmitter(emitterSource.value());
    if(!emitter) {
        ERROR(error(filePath + " failed to parse"));
        return nullptr;
    }

    return std::make_shared<EmitterData>(emitter.value());
}

auto EmitterLoader::parseEmitter(const std::string& source)
    -> std::expected<EmitterData, JSONParserError> {
    json emitterJSON;

    try {
        emitterJSON = json::parse(source);
    } catch(const json::exception& e) {
        ERROR(error(std::string(e.what())));
        return std::unexpected(JSONParserError::PARSE);
    }

    EmitterData emitter;
    if(!get<std::string>(emitterJSON, "particle_path", true, emitter.particleDataFile)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(emitterJSON, "spawn_rate", true, emitter.spawnRate)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<u32>(emitterJSON, "max_particles", true, emitter.maxParticles)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    std::string emitterShape;
    if(!get<std::string>(emitterJSON, "shape", true, emitterShape)) {
        return std::unexpected(JSONParserError::PARSE);
    }
    emitter.shape =
        magic_enum::enum_cast<EmitterShape>(emitterShape).value_or(EmitterShape::UNKNOWN);

    if(emitter.shape == EmitterShape::ARC) {
        if(!get<f32>(emitterJSON, "arc", true, emitter.arc)) {
            return std::unexpected(JSONParserError::PARSE);
        }
    }

    if(!get<f32>(emitterJSON, "direction", true, emitter.directionInDegrees)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    return emitter;
}

auto EmitterLoader::error(const std::string& msg, const std::string& parent) -> std::string {
    std::string error = "[EMITTER LOADER]: ";
    if(!parent.empty()) {
        error += '(' + parent + ") ";
    }
    return error + msg;
}
