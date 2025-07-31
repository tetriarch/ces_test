#include "particle_loader.hpp"

#include "../file_io.hpp"

auto ParticleLoader::load(AssetManager& assetManager, const std::string& filePath) -> IAssetPtr {
    auto particleSource = FileIO::readTextFile(filePath);
    if(!particleSource) {
        ERROR(error(filePath + " " + particleSource.error().message()));
        return nullptr;
    }

    auto particle = parseParticle(particleSource.value());
    if(!particle) {
        ERROR(error(filePath + " failed to parse"));
        return nullptr;
    }

    return std::make_shared<ParticleData>(particle.value());
}
auto ParticleLoader::parseParticle(const std::string& source)
    -> std::expected<ParticleData, JSONParserError> {
    json particleJSON;

    try {
        particleJSON = json::parse(source);
    } catch(const json::exception& e) {
        ERROR(error(std::string(e.what())));
        return std::unexpected(JSONParserError::PARSE);
    }

    ParticleData particle;

    if(!get<std::string>(particleJSON, "texture_path", true, particle.textureFilePath)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "lifetime_min", true, particle.minLifeTime)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "lifetime_max", true, particle.maxLifeTime)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "scale_min_start", true, particle.minStartScale)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "scale_max_start", true, particle.maxStartScale)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "scale_min_end", true, particle.minEndScale)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "scale_max_end", true, particle.maxEndScale)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "alpha_min_start", true, particle.minStartAlpha)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "alpha_max_start", true, particle.maxStartAlpha)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "alpha_min_end", true, particle.minEndAlpha)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "alpha_max_end", true, particle.maxEndAlpha)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "speed_min", true, particle.minSpeed)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    if(!get<f32>(particleJSON, "speed_max", true, particle.maxSpeed)) {
        return std::unexpected(JSONParserError::PARSE);
    }

    f32 minAngularVelocity = 0.0f;
    if(!get<f32>(particleJSON, "angular_velocity_min", true, minAngularVelocity)) {
        return std::unexpected(JSONParserError::PARSE);
    }
    particle.minAngularVelocity = math::radians(minAngularVelocity);

    f32 maxAngularVelocity = 0.0f;
    if(!get<f32>(particleJSON, "angular_velocity_max", true, maxAngularVelocity)) {
        return std::unexpected(JSONParserError::PARSE);
    }
    particle.maxAngularVelocity = math::radians(maxAngularVelocity);

    return particle;
}

auto ParticleLoader::error(const std::string& msg, const std::string& parent) const -> std::string {
    std::string error = "[PARTICLE LOADER]: ";
    if(!parent.empty()) {
        error += '(' + parent + ") ";
    }
    return error + msg;
}
