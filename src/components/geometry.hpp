#pragma once

#include "../component.hpp"
#include "../math.hpp"
#include "../texture.hpp"
#include "../utils.hpp"

#include <SDL3/SDL.h>

class GeometryComponent : public Component<GeometryComponent> {

public:
    void attach() override;
    void setTextureFilePath(const std::string& filePath);
    void setRect(const Rect& rect);
    void render(SDL_Renderer* renderer) override;

private:
    std::string textureFilePath_;
    Rect rect_;
    std::shared_ptr<Texture> texture_;
};