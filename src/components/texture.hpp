#pragma once

#include "../component.hpp"
#include "../utils.hpp"

class TextureComponent : public Component<TextureComponent> {

public:
    auto describe() -> std::string override;
    void setFilePath(const std::string& filePath);

private:
    std::string filePath_;
};