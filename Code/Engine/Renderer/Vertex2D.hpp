#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Rgba.hpp"

struct Vertex2D {
public:
    Vector2 position;
    Rgba color;
    Vector2 texCoords;
    Vertex2D(const Vector2& pos = Vector2(0.0f, 0.0f), const Rgba& color = Rgba::WHITE, const Vector2& tex = Vector2(0.0f, 0.0f))
        : position(pos), color(color), texCoords(tex)
    {
        /* DO NOTHING */
    }

protected:
private:
	
};