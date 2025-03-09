#pragma once

#include "i_asset.hpp"

#include <SDL3/SDL.h>

class Texture : public IAsset {

public:
    void setTexture(SDL_Texture* texture);
    auto get() -> SDL_Texture* const;

private:
    SDL_Texture* texture_;
};