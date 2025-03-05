#include "texture.hpp"

auto TextureComponent::describe() -> std::string {
    return "I hold texture - " + filePath_;
}

void TextureComponent::setFilePath(const std::string& filePath) {

    filePath_ = filePath;
}