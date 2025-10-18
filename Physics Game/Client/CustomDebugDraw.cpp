#include "CustomDebugDraw.h"
#include "Common.h"


//draws the outline of a rectangle
void CustomDebugDraw::DrawOutlineRect(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    Color raylibColor = {
        static_cast<unsigned char>(color.r * 255),
        static_cast<unsigned char>(color.g * 255),
        static_cast<unsigned char>(color.b * 255),
        255 // Set alpha value to 255 (fully opaque)
    };

    // Loop through each vertex in the polygon
    for (int i = 0; i < vertexCount; i++) {
        // Get the current vertex
        const b2Vec2& v1 = vertices[i];
        // Get the next vertex, or loop back to the first if we're at the last one
        const b2Vec2& v2 = vertices[(i + 1) % vertexCount];

        // Convert Box2D coordinates to Raylib screen coordinates for both vertices
        Vector2 start = { v1.x * PIXELS_PER_METER, GetScreenHeight() - v1.y * PIXELS_PER_METER };
        Vector2 end = { v2.x * PIXELS_PER_METER, GetScreenHeight() - v2.y * PIXELS_PER_METER };

        // Draw a line between the two vertices
        DrawLineV(start, end, raylibColor);
    }
}

//draws a filled rectangle
void CustomDebugDraw::DrawFillRect(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    // Calculate the centroid of the polygon
    float centroidX = 0, centroidY = 0;
    for (int i = 0; i < vertexCount; i++) {
        centroidX += vertices[i].x;
        centroidY += vertices[i].y;
    }
    centroidX /= vertexCount;
    centroidY /= vertexCount;

    // Convert centroid to Raylib's screen coordinates
    Vector2 centroid = { centroidX * PIXELS_PER_METER, GetScreenHeight() - centroidY * PIXELS_PER_METER };

    Color raylibColor = {
        static_cast<unsigned char>(color.r * 255),
        static_cast<unsigned char>(color.g * 255),
        static_cast<unsigned char>(color.b * 255),
        255 // Set alpha value to 255 (fully opaque)
    };

    // Draw triangles between the centroid and each edge of the polygon
    for (int i = 0; i < vertexCount; i++) {
        // Get the current and next vertex
        const b2Vec2& v1 = vertices[i];
        const b2Vec2& v2 = vertices[(i + 1) % vertexCount];

        // Convert Box2D coordinates to Raylib screen coordinates
        Vector2 start = { v1.x * PIXELS_PER_METER, GetScreenHeight() - v1.y * PIXELS_PER_METER };
        Vector2 end = { v2.x * PIXELS_PER_METER, GetScreenHeight() - v2.y * PIXELS_PER_METER };

        // Draw the triangle
        Vector2 points[3] = { start, end, centroid };
        DrawTriangle(points[0], points[1], points[2], raylibColor);
    }
}

//this is called by Box2D when it wants to draw a polygon shape (we are assuming a rectangle to keep things simple)
void CustomDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    if (vertexCount == 4) { //assume rectangles
        if (rectDrawFill) {
            DrawFillRect(vertices, vertexCount, color);
        }
        else {
            DrawOutlineRect(vertices, vertexCount, color);
        }
    }
}

//this is called by Box2D when it wants to draw a circle
void CustomDebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
    Vector2 screenCenter = { center.x * PIXELS_PER_METER, GetScreenHeight() - center.y * PIXELS_PER_METER };

    // Convert Box2D world radius to Raylib screen radius
    float screenRadius = radius * PIXELS_PER_METER;

    Color raylibColor = {
           static_cast<unsigned char>(color.r * 255),
           static_cast<unsigned char>(color.g * 255),
           static_cast<unsigned char>(color.b * 255),
           255 // Set alpha value to 255 (fully opaque)
    };

    // Draw the circle lines
    if (circleDrawFill)
        DrawCircleV(screenCenter, screenRadius, raylibColor);
    else
        DrawCircleLines(screenCenter.x, screenCenter.y, screenRadius, raylibColor);
}

//this function can be used to choose whether to draw filled shapes or outlines
void CustomDebugDraw::DrawSolidOrOutline(DrawType drawType) {
	rectDrawFill = drawType == DRAW_FILL;
	circleDrawFill = drawType == DRAW_FILL;
}

