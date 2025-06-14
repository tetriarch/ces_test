#include "texture_loader.hpp"

#include <SDL3_image/SDL_image.h>

#include "../log.hpp"
#include "../renderer.hpp"

TextureLoader::TextureLoader(std::shared_ptr<Renderer> renderer) : renderer_(renderer) {
}

auto TextureLoader::load(AssetManager& assetManager, const std::string& assetPath) -> IAssetPtr {
    auto renderer = renderer_.lock();
    if(!renderer) {
        ERROR("[TEXTURE LOADER]: failed to access renderer handle to load texture");
        return nullptr;
    }

    SDL_Texture* texture = IMG_LoadTexture(renderer->handle(), assetPath.c_str());
    if(!texture) {
        std::string error = SDL_GetError();
        ERROR("[TEXTURE LOADER]: " + error);
        return nullptr;
    }
    Texture result;
    result.setTexture(texture);

    return std::make_shared<Texture>(result);
}
