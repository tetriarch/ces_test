#include "life.hpp"

const Life& LifeComponent::getLife() const {
    return life_;
}



void LifeComponent::setLife(const Life& life) {
    life_ = life;
}



void LifeComponent::update() {

}



auto LifeComponent::describe() -> std::string {
    return "I have " + std::to_string(life_.current) + "/" + std::to_string(life_.max) + " life";
}