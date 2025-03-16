#pragma once

#include "../component.hpp"
#include "../utils.hpp"
#include "../texture.hpp"

#include <SDL3/SDL.h>

class TextureComponent : public Component<TextureComponent> {

public:
    void attach() override;
    void setFilePath(const std::string& filePath);
    void render(SDL_Renderer* renderer) override;

private:
    std::string filePath_;
    std::shared_ptr<Texture> texture_;
};