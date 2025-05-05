#pragma once

#include "../component.hpp"
#include "../math.hpp"
#include "../texture.hpp"
#include "../utils.hpp"

#include <SDL3/SDL.h>

enum class GeometrySizeDeterminant {
    TARGET,
    NONE
};

struct GeometryData {

    Rect rect;
    GeometrySizeDeterminant sizeDeterminant;
};

class GeometryComponent : public Component<GeometryComponent> {

public:
    void attach() override;
    void setTextureFilePath(const std::string& filePath);
    void setGeometryData(const GeometryData& geometryData);
    void postUpdate(f32 dt) override;
    void render(SDL_Renderer* renderer) override;
    Rect rect() const;

#ifdef DEBUG
    void handleEvents(const SDL_Event& event) override;
#endif

private:
    std::string textureFilePath_;
    std::shared_ptr<Texture> texture_;
    Rect rect_;
    GeometryData geometryData_;

#ifdef DEBUG
    bool showCollisions_{true};
#endif
};