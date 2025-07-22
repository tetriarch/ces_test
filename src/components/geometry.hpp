#pragma once

#include <SDL3/SDL.h>

#include "../component.hpp"
#include "../math.hpp"
#include "../utils.hpp"

enum class GeometrySizeDeterminant { TARGET, NONE };

struct GeometryData {
    Rect rect;
    GeometrySizeDeterminant sizeDeterminant;
};

class GeometryComponent : public Component<GeometryComponent> {
public:
    s32 updatePriority() override {
        return 3;
    }
    void setTextureFilePath(const std::string& filePath);
    void setGeometryData(const GeometryData& geometryData);
    void postUpdate(f32 dt) override;
    void render(std::shared_ptr<Renderer> renderer) override;
    Rect rect() const;

private:
    std::string textureFilePath_;
    Rect rect_;
    GeometryData geometryData_;
};
