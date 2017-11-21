#pragma once

#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

#include "Engine/Core/Rgba.hpp"

#include "Engine/Renderer/Vertex2D.hpp"

struct Vertex3D {
public:
    Vector3 position;
    Rgba color;
    Vector2 texCoords;
    Vector3 normal;
    Vector3 tangent;
    Vector3 bitangent;
    IntVector4 bone_indices;
    Vector4 bone_weights;

    //pos: vertex position
    //color: vertex color and alpha
    //tex: vertex texture coords
    //n: vertex normal
    //t: vertex tangent
    //bt: vertex bitangent
    //bi: bone indices
    //bw: bone weights
    Vertex3D(const Vector3& pos = Vector3::ZERO, const Rgba& color = Rgba::WHITE, const Vector2& tex = Vector2::ZERO, const Vector3& n = Vector3::ZERO, const Vector3& t = Vector3::ZERO, const Vector3& bt = Vector3::ZERO, const IntVector4& bi = IntVector4(1, 0, 0, 0), const Vector4& bw = Vector4(1.0f, 0.0f, 0.0f, 0.0f));

    //Create a Vertex3D from a Vertex2D
    Vertex3D(const Vertex2D& vertex2d);

protected:
private:

};

inline Vertex3D::Vertex3D(const Vector3& pos /*= Vector3::ZERO*/, const Rgba& color /*= Rgba::WHITE*/, const Vector2& tex /*= Vector2::ZERO*/, const Vector3& n /*= Vector3::ZERO*/, const Vector3& t /*= Vector3::ZERO*/, const Vector3& bt /*= Vector3::ZERO*/, const IntVector4& bi /*= IntVector4(1, 0, 0, 0)*/, const Vector4& bw /*= Vector4(1.0f, 0.0f, 0.0f, 0.0f)*/)
    : position(pos), color(color), texCoords(tex), normal(n), tangent(t), bitangent(bt), bone_indices(bi), bone_weights(bw) {
    /* DO NOTHING */
}

inline Vertex3D::Vertex3D(const Vertex2D& vertex2d)
    : position(Vector3(vertex2d.position.x, vertex2d.position.y, 0.0f)), color(vertex2d.color), texCoords(vertex2d.texCoords), normal(-Vector3::Z_AXIS), tangent(Vector3::ZERO), bitangent(Vector3::ZERO), bone_indices(IntVector4(1, 0, 0, 0)), bone_weights(Vector4(1.0f, 0.0f, 0.0f, 0.0f)) {
    /* DO NOTHING */
}
