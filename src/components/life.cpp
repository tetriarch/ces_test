#include "life.hpp"

const Life& LifeComponent::life() const {
    return life_;
}

void LifeComponent::setLife(const Life& life) {
    life_ = life;
}