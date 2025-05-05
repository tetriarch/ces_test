#include "log.hpp"
#include "texture_loader.hpp"

#include <SDL3_image/SDL_image.h>

TextureLoader::TextureLoader(SDL_Renderer* renderer) : renderer_(renderer) {

}

auto TextureLoader::load(AssetManager& assetManager, const std::string& assetPath) -> IAssetPtr {

    SDL_Texture* texture = IMG_LoadTexture(renderer_, assetPath.c_str());
    if(!texture) {
        std::string error = SDL_GetError();
        ERROR("[TEXTURE LOADER]: " + error);
        return nullptr;
    }
    Texture result;
    result.setTexture(texture);

    return std::make_shared<Texture>(result);
}