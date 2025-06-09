#include "texture.hpp"

void Texture::setTexture(SDL_Texture* texture) {
    texture_ = texture;
}

auto Texture::get() const -> SDL_Texture* {
    return texture_;
}
