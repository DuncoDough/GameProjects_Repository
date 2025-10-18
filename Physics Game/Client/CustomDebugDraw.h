#pragma once

#include <box2d/box2d.h>
#include <raylib.h>

class CustomDebugDraw : public b2Draw { //https://www.iforce2d.net/b2dtut/debug-draw

private:
    bool rectDrawFill = true;
    bool circleDrawFill = true;

    void DrawOutlineRect(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    void DrawFillRect(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override {}
    void DrawTransform(const b2Transform& xf) override {}
    void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override {}
    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override {}
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {}

public:
    enum DrawType {
        DRAW_OUTLINE,
        DRAW_FILL
    };

    CustomDebugDraw() = default;
    ~CustomDebugDraw() = default;

    void DrawSolidOrOutline(DrawType drawType); //drawType is either DRAW_FILL or DRAW_OUTLINE


};



