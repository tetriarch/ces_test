#include "player_control.hpp"

#include "spell_book.hpp"
#include "velocity.hpp"

Vec2 PlayerControlComponent::getMousePosition() {
    return {mousePosition_.x, mousePosition_.y};
}

void PlayerControlComponent::handleEvents(const SDL_Event& event) {
    if(event.type == SDL_EVENT_MOUSE_MOTION) {
        mousePosition_.x = event.motion.x;
        mousePosition_.y = event.motion.y;
    }

    if(event.type == SDL_EVENT_KEY_DOWN) {
        if(event.key.key == SDLK_W) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setMotion(MovementDirection::NORTH);
        }
        if(event.key.key == SDLK_A) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setMotion(MovementDirection::WEST);
        }
        if(event.key.key == SDLK_S) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setMotion(MovementDirection::SOUTH);
        }
        if(event.key.key == SDLK_D) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setMotion(MovementDirection::EAST);
        }
        if(event.key.key == SDLK_SPACE) {
            auto spellBook = entity()->component<SpellBookComponent>();
            if(spellBook) spellBook->interruptCasting();
        }
    }
    if(event.type == SDL_EVENT_KEY_UP) {
        if(event.key.key == SDLK_W) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->stopMotion(MovementDirection::NORTH);
        }
        if(event.key.key == SDLK_A) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->stopMotion(MovementDirection::WEST);
        }
        if(event.key.key == SDLK_S) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->stopMotion(MovementDirection::SOUTH);
        }
        if(event.key.key == SDLK_D) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->stopMotion(MovementDirection::EAST);
        }
        if(event.key.key == SDLK_Q) {
            auto spellBook = entity()->component<SpellBookComponent>();
            if(spellBook) spellBook->castSpell(0, mousePosition_);
        }
        if(event.key.key == SDLK_E) {
            auto spellBook = entity()->component<SpellBookComponent>();
            if(spellBook) spellBook->castSpell(1, mousePosition_);
        }
        if(event.key.key == SDLK_R) {
            auto spellBook = entity()->component<SpellBookComponent>();
            if(spellBook) spellBook->castSpell(2, mousePosition_);
        }
        if(event.key.key == SDLK_F) {
            auto spellBook = entity()->component<SpellBookComponent>();
            if(spellBook) spellBook->castSpell(3, mousePosition_);
        }
    }
}

void PlayerControlComponent::update(f32 dt) {
    Transform t = entity()->transform();
    Vec2 direction = mousePosition_ - t.position;
    direction = direction.normalized();
    t.rotationInDegrees = angleFromDirection(direction);
    entity()->setTransform(t);
}
