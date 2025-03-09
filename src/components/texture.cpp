#include "texture.hpp"
#include "../asset_manager.hpp"
#include "../entity.hpp"

auto TextureComponent::describe() -> std::string {
    return "I hold texture - " + filePath_;
}

void TextureComponent::attach() {
    auto am = AssetManager::get();
    texture_ = am->load<Texture>(filePath_);
}

void TextureComponent::setFilePath(const std::string& filePath) {

    filePath_ = filePath;
}

void TextureComponent::render(SDL_Renderer* renderer) {

    assert(!texture_.expired());

    Transform transform = entity()->transform();
    SDL_FRect rect;
    rect.x = transform.position.x;
    rect.y = transform.position.y;
    rect.w = texture_.lock()->get()->w;
    rect.h = texture_.lock()->get()->h;

    SDL_RenderTexture(renderer, texture_.lock()->get(), nullptr, &rect);
}
