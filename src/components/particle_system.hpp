#pragma once
#include "../component.hpp"
#include "../i_asset.hpp"
#include "../math.hpp"
#include "../random_number_generator.hpp"

struct ParticleData : IAsset {
    std::string textureFilePath;

    // lifetime
    f32 minLifeTime;
    f32 maxLifeTime;

    // scale
    f32 minStartScale;
    f32 maxStartScale;
    f32 minEndScale;
    f32 maxEndScale;

    // alpha
    f32 minStartAlpha;
    f32 maxStartAlpha;
    f32 minEndAlpha;
    f32 maxEndAlpha;

    // speed
    f32 minSpeed;
    f32 maxSpeed;

    // angularVelocity
    f32 minAngularVelocity;
    f32 maxAngularVelocity;
};

struct Particle {
    std::string textureFilePath;
    bool dead{true};
    Vec2 position;
    Vec2 velocity;
    f32 maxDuration;
    f32 startDuration;
    f32 currentDuration;
    f32 startAlpha;
    f32 currentAlpha;
    f32 finalAlpha;
    f32 startScale;
    f32 currentScale;
    f32 finalScale;
    f32 speed;
    f32 angleInDegrees;
    f32 angularVelocity;
};

enum class EmitterShape { DOT, CIRCLE, LINE, ARC, UNKNOWN };

struct EmitterData : IAsset {
    std::string particleDataFile;
    f32 spawnRate;
    u32 maxParticles;
    EmitterShape shape;
    f32 arc;
    f32 directionInDegrees;
};

class Emitter {
public:
    void setData(std::shared_ptr<EmitterData> emitterData);
    void spawnParticle(Transform entityTransform);
    std::vector<Particle>& particles();
    bool canSpawn();
    void updateSpawnTime(const f32 dt);

private:
    f32 cooldown();

private:
    f32 spawnTime_{0};
    std::shared_ptr<EmitterData> emitterData_;
    std::vector<Particle> particles_;
    RandomNumberGenerator rng_;
};

class ParticleSystemComponent : public Component<ParticleSystemComponent> {
public:
    void attach() override;
    void addEmitterFiles(const std::vector<std::string>& emitterFilePaths);
    void setEmitting(bool state);
    void update(const f32 dt) override;
    void render(std::shared_ptr<Renderer> renderer) override;

private:
    bool active_{true};
    std::vector<std::string> emitterFilePaths_;
    std::vector<Emitter> emitters_;
};
