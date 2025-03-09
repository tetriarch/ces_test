#include "texture.hpp"

void Texture::setTexture(SDL_Texture* texture) {

    texture_ = texture;
}

auto Texture::get() -> SDL_Texture* const {

    return texture_;
}
