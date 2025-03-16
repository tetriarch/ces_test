#include "texture.hpp"
#include "../asset_manager.hpp"
#include "../entity.hpp"

void TextureComponent::attach() {
    auto am = AssetManager::get();
    texture_ = am->load<Texture>(filePath_);
}

void TextureComponent::setFilePath(const std::string& filePath) {

    filePath_ = filePath;
}

void TextureComponent::render(SDL_Renderer* renderer) {

    if(!texture_) {
        ERROR_ONCE("[TEXTURE COMPONENT]: texture is empty");
        return;
    }

    Transform transform = entity()->transform();
    SDL_FRect rect;
    rect.x = transform.position.x - texture_->get()->w / 2.0;
    rect.y = transform.position.y - texture_->get()->h / 2.0;
    rect.w = texture_->get()->w;
    rect.h = texture_->get()->h;

    SDL_RenderTextureRotated(renderer, texture_->get(), nullptr, &rect, transform.rotationInDegrees, nullptr, SDL_FlipMode::SDL_FLIP_NONE);
}
