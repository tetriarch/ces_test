#include "player_control.hpp"
#include "components.hpp"

auto PlayerControlComponent::describe() -> std::string {

    return "I handle input from keyboard and mouse to control the player";
}

void PlayerControlComponent::handleEvents(const SDL_Event& event) {

    if(event.type == SDL_EVENT_KEY_DOWN) {
        if(event.key.key == SDLK_W) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setDirection(Vec2(0, -1));
        }
        if(event.key.key == SDLK_A) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setDirection(Vec2(-1, 0));
        }
        if(event.key.key == SDLK_S) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setDirection(Vec2(0, 1));
        }
        if(event.key.key == SDLK_D) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setDirection(Vec2(1, 0));
        }
    }
    if(event.type == SDL_EVENT_KEY_UP) {
        if(event.key.key == SDLK_W) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setDirection(Vec2(0, 1));
        }
        if(event.key.key == SDLK_A) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setDirection((Vec2(1, 0)));
        }
        if(event.key.key == SDLK_S) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setDirection((Vec2(0, -1)));
        }
        if(event.key.key == SDLK_D) {
            auto velocity = entity()->component<VelocityComponent>();
            if(velocity) velocity->setDirection((Vec2(-1, 0)));
        }
    }
}
