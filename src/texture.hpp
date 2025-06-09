#pragma once

#include <SDL3/SDL.h>

#include "i_asset.hpp"

class Texture : public IAsset {
public:
    void setTexture(SDL_Texture* texture);
    auto get() const -> SDL_Texture*;

private:
    SDL_Texture* texture_;
};
