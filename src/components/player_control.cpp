#include "player_control.hpp"
#include "components.hpp"

auto PlayerControlComponent::describe() -> std::string {

    return "I handle input from keyboard and mouse to control the player";
}

void PlayerControlComponent::handleEvents(const SDL_Event& event) {

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
    }
}
