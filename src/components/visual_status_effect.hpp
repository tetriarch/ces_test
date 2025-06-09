#include "../component.hpp"
#include "../math.hpp"

#pragma once

class VisualStatusEffectComponent : public Component<VisualStatusEffectComponent> {
public:
    void setTextureFile(const std::string& textureFilePath);
    void setRect(const Rect& rect);

    void render(std::shared_ptr<Renderer> renderer) override;

private:
    std::string textureFilePath_;
    Rect rect_;
};
