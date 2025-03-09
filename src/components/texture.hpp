#pragma once

#include "../component.hpp"
#include "../utils.hpp"
#include "../texture.hpp"

#include <SDL3/SDL.h>

class TextureComponent : public Component<TextureComponent> {

public:
    auto describe() -> std::string override;
    void attach() override;
    void setFilePath(const std::string& filePath);
    void render(SDL_Renderer* renderer) override;

private:
    std::string filePath_;
    std::weak_ptr<Texture> texture_;
};