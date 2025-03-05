#include "player_control.hpp"

auto PlayerControlComponent::describe() -> std::string {

    return "I handle input from keyboard and mouse to control the player";
}

void PlayerControlComponent::handleInput() {
    // handle input here
}
