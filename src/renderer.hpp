#pragma once
#include <SDL3/SDL.h>

#include <functional>

#include "math.hpp"

enum class Strata { TERRAIN = 1, ENTITY = 2, EFFECT = 3, UI = 4, DEB = 5 };

class Renderer {
public:
    Renderer(SDL_Renderer* renderer);
    void destroy();
    static Renderer init(SDL_Window* window);

public:
    SDL_Renderer* handle();
    void toogleVsync(bool toggle);
    void queueRenderTexture(
        Strata strata, const std::string& textureName, const Rect& sRect, const Rect& dRect);
    void queueRenderTextureRotated(Strata strata, const std::string& textureName, const Rect& sRect,
        const Rect& dRect, const Vec2& pivot, f32 angleInDegrees);
    void queueRenderTextureRotated(Strata strata, const std::string& textureName, Vec2 position,
        f32 angleInDegrees, f32 scale, f32 alpha);

    void queueRenderRect(Strata strata, const Rect& rect, u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 255);
    void queueRenderFilledRect(
        Strata strata, const Rect& rect, u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 255);

    void queueRenderLine(Strata strata, const Line& line, u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 255);
    void executeRenderCalls();
    void present();
    void clear();

private:
    void renderGroup(const std::vector<std::function<void()>>& group);
    void queueRenderCall(Strata strata, std::function<void()> renderCall);

private:
    SDL_Renderer* renderer_;

private:
    std::vector<std::function<void()>> terrain_;
    std::vector<std::function<void()>> entities_;
    std::vector<std::function<void()>> effects_;
    std::vector<std::function<void()>> ui_;
#ifdef DEBUG
    std::vector<std::function<void()>> debug_;
#endif
};
