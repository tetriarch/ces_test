#include "particle_system.hpp"

#include <numbers>

#include "../asset_manager.hpp"
#include "../entity.hpp"
#include "../math.hpp"
#include "../renderer.hpp"

void Emitter::setData(std::shared_ptr<EmitterData> emitterData) {
    emitterData_ = emitterData;
}

void Emitter::spawnParticle(Transform entityTransform) {
    auto am = AssetManager::get();
    auto particleData = am->load<ParticleData>(emitterData_->particleDataFile);
    if(!particleData) {
        ERROR_ONCE("[EMITTER]: failed to acquire particle data");
        return;
    }
    Particle particle;
    particle.textureFilePath = particleData->textureFilePath;
    particle.position = entityTransform.position;
    particle.startAlpha = rng_.getFloat(particleData->minStartAlpha, particleData->maxStartAlpha);
    particle.currentAlpha = particle.startAlpha;

    particle.finalAlpha = rng_.getFloat(particleData->minEndAlpha, particleData->maxEndAlpha);

    particle.angle = entityTransform.rotation + emitterData_->directionAngle;
    particle.angle = particle.angle > std::numbers::pi_v<f32> * 2
                         ? particle.angle - std::numbers::pi_v<f32> * 2
                         : particle.angle;

    particle.speed = rng_.getFloat(particleData->minSpeed, particleData->maxSpeed);
    particle.startScale = rng_.getFloat(particleData->minStartScale, particleData->maxStartScale);

    particle.currentScale = particle.startScale;

    particle.finalScale = rng_.getFloat(particleData->minEndScale, particleData->maxEndScale);

    particle.startDuration = rng_.getFloat(particleData->minLifeTime, particleData->maxLifeTime);
    particle.currentDuration = particle.startDuration;

    particle.maxDuration = particleData->maxLifeTime;
    particle.angularVelocity =
        rng_.getFloat(particleData->minAngularVelocity, particleData->maxAngularVelocity);

    if(emitterData_->shape == EmitterShape::ARC) {
        f32 angleVariance = rng_.getFloat(-emitterData_->arc / 2.0f, emitterData_->arc / 2.0f);
        particle.angle += angleVariance;
    }

    particle.velocity = {
        static_cast<f32>(cos(particle.angle)), static_cast<f32>(sin(particle.angle))};

    // find dead particle and replace it
    for(auto&& p : particles_) {
        if(p.dead) {
            particle.dead = false;
            p = particle;
            spawnTime_ -= cooldown();
            break;
        }
    }
}

auto Emitter::particles() -> std::vector<Particle>& {
    return particles_;
}

void Emitter::updateSpawnTime(const f32 dt) {
    spawnTime_ += dt;
}

bool Emitter::canSpawn() {
    if(spawnTime_ > cooldown()) {
        return true;
    }
    return false;
}

f32 Emitter::cooldown() {
    return 1.0f / emitterData_->spawnRate;
}

void ParticleSystemComponent::attach() {
    auto am = AssetManager::get();
    for(auto& ef : emitterFilePaths_) {
        auto emitterData = am->load<EmitterData>(ef);
        if(emitterData) {
            Emitter emitter;
            emitter.setData(emitterData);
            emitter.particles().resize(emitterData->maxParticles);
            emitters_.push_back(emitter);
        }
    }
}

void ParticleSystemComponent::addEmitterFiles(const std::vector<std::string>& emitterFilePaths) {
    if(emitterFilePaths.empty()) {
        ERROR(
            "[PARTICLE SYSTEM COMPONENT]: " + entity()->name() +
            " failed to add emitter files, the list is empty");
        return;
    }

    emitterFilePaths_ = emitterFilePaths;
}

void ParticleSystemComponent::setEmitting(bool state) {
    active_ = state;
}

void ParticleSystemComponent::update(const f32 dt) {
    for(auto& emitter : emitters_) {
        // is time to spawn? are we active?
        if(emitter.canSpawn() && active_) {
            emitter.spawnParticle(entity()->transform());
        }
        for(auto& particle : emitter.particles()) {
            if(!particle.dead) {
                particle.position += particle.velocity * particle.speed * dt;
                if(particle.currentDuration < particle.maxDuration) {
                    f32 t = math::rlerp(
                        particle.startDuration, particle.maxDuration, particle.currentDuration);
                    particle.currentScale = math::lerp(particle.startScale, particle.finalScale, t);
                    particle.currentAlpha = math::lerp(particle.startAlpha, particle.finalAlpha, t);
                    particle.angle += particle.angularVelocity * dt;
                    particle.currentDuration += dt;
                }
                if(particle.currentDuration > particle.maxDuration) {
                    particle.dead = true;
                }
            }
            emitter.updateSpawnTime(dt);
        }
    }
}

void ParticleSystemComponent::render(std::shared_ptr<Renderer> renderer) {
    for(auto& emitter : emitters_) {
        for(auto& particle : emitter.particles()) {
            if(!particle.dead) {
                renderer->queueRenderTextureRotated(
                    Strata::EFFECT, particle.textureFilePath, particle.position, particle.angle,
                    particle.currentScale, particle.currentAlpha);
            }
        }
    }
}
