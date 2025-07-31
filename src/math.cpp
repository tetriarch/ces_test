#include "math.hpp"

Rect Rect::rotate(f32 angle, const Vec2* pivotPoint) {
    const f32 c = cosf(angle);
    const f32 s = sinf(angle);

    Vec2 min = {x, y};
    Vec2 max = {x + w, y + h};

    Vec2 center;

    if(pivotPoint) {
        center = *pivotPoint;
    } else {
        center.x = x + w / 2.0f;
        center.y = y + h / 2.0f;
    }

    f32 pivotX = center.x + x;
    f32 pivotY = center.y + y;

    f32 sMinX, sMinY, sMaxX, sMaxY, cMinX, cMinY, cMaxX, cMaxY;

    sMinX = s * (min.x - pivotX);
    sMinY = s * (min.y - pivotY);
    sMaxX = s * (max.x - pivotX);
    sMaxY = s * (max.y - pivotY);

    cMinX = c * (min.x - pivotX);
    cMinY = c * (min.y - pivotY);
    cMaxX = c * (max.x - pivotX);
    cMaxY = c * (max.y - pivotY);

    Vec2 rotatedCorners[4] = {
        Vec2((cMinX - sMinY) + pivotX, (sMinX + cMinY) + pivotY),
        Vec2((cMaxX - sMinY) + pivotX, (sMaxX + cMinY) + pivotY),
        Vec2((cMaxX - sMaxY) + pivotX, (sMaxX + cMaxY) + pivotY),
        Vec2((cMinX - sMaxY) + pivotX, (sMinX + cMaxY) + pivotY)};

    f32 minX = rotatedCorners[0].x;
    f32 minY = rotatedCorners[0].y;
    f32 maxX = minX;
    f32 maxY = minY;

    for(u32 i = 1; i < 4; i++) {
        minX = std::min(minX, rotatedCorners[i].x);
        maxX = std::max(maxX, rotatedCorners[i].x);
        minY = std::min(minY, rotatedCorners[i].y);
        maxY = std::max(maxY, rotatedCorners[i].y);
    }

    x = minX;
    y = minY;
    w = maxX - minX;
    h = maxY - minY;

    return {x, y, w, h};
}

Box Box::rotate(f32 angle, const Vec2* pivotPoint) {
    f32 cosTheta = cosf(angle);
    f32 sinTheta = sinf(angle);

    f32 pivotX = 0.0f;
    f32 pivotY = 0.0f;

    if(pivotPoint) {
        pivotX = pivotPoint->x;
        pivotY = pivotPoint->y;
    }

    Vec2 corners[4] = {tl, tr, br, bl};

    for(u32 i = 0; i < 4; i++) {
        f32 dx = corners[i].x - pivotX;
        f32 dy = corners[i].y - pivotY;

        corners[i].x = pivotX + dx * cosTheta - dy * sinTheta;
        corners[i].y = pivotY + dx * sinTheta + dy * cosTheta;
    }

    tl = corners[0];
    tr = corners[1];
    br = corners[2];
    bl = corners[3];

    return {tl, tr, br, bl};
}

Line Line::rotate(f32 angle, const Vec2* pivotPoint) {
    const f32 c = cosf(angle);
    const f32 s = sinf(angle);

    Vec2 center;
    if(pivotPoint) {
        center = *pivotPoint;
    } else {
        center = (p1 + p2) * 0.5f;
    }

    Vec2 newP1, newP2;
    newP1.x = c * (p1.x - center.x) - s * (p1.y - center.y) + center.x;
    newP1.y = s * (p1.x - center.x) + c * (p1.y - center.y) + center.y;

    newP2.x = c * (p2.x - center.x) - s * (p2.y - center.y) + center.x;
    newP2.y = s * (p2.x - center.x) + c * (p2.y - center.y) + center.y;

    p1 = newP1;
    p2 = newP2;

    return {p1, p2};
}
