#pragma once

#include <vector>

#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/RHI/RHITypes.hpp"

#include "Engine/Core/FileUtils.hpp"

class SimpleRenderer;

struct draw_instruction_t {
    PrimitiveType type;
    unsigned int start_index;
    unsigned int count;
    bool uses_index_buffer;
};

bool operator==(const draw_instruction_t& a, const draw_instruction_t& b);
bool operator!=(const draw_instruction_t& a, const draw_instruction_t& b);

class MeshBuilder {
public:
    MeshBuilder();
    MeshBuilder(const std::vector<Vertex3D>& verts, const std::vector<unsigned int> indcs);
	~MeshBuilder();

    std::vector<Vertex3D> verticies;
    std::vector<unsigned int> indicies;
    std::vector<draw_instruction_t> draw_instructions;

    void Begin(const PrimitiveType& type, bool hasIndexBuffer = true);
    void End();
    void Clear();

    void SetTangent(const Vector3& tangent);
    void SetBitangent(const Vector3& bitangent);
    void SetNormal(const Vector3& normal);
    void SetColor(const Rgba& color);
    void SetUV(const Vector2& uv);
    void SetBoneWeights(const Vector4& bone_weights);
    void SetBoneIndices(const IntVector4& bone_indices);

    std::size_t AddVertex(const Vector3& position);

    bool write(FileUtils::BinaryStream& stream) const;
    bool read(FileUtils::BinaryStream& stream);

protected:
private:
    Vertex3D vertex_prototype;
    draw_instruction_t m_current_draw_instruction;
};