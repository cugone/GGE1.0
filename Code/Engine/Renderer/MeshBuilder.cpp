#include "Engine/Renderer/MeshBuilder.hpp"

#include "Engine/Core/FileUtils.hpp"

bool operator==(const draw_instruction_t& a, const draw_instruction_t& b) {
    return a.type == b.type && a.uses_index_buffer == b.uses_index_buffer;
}

bool operator!=(const draw_instruction_t& a, const draw_instruction_t& b) {
    return !(a == b);
}

MeshBuilder::MeshBuilder()
    : verticies{}
    , indicies{}
    , draw_instructions{}
    , vertex_prototype(Vector3::ZERO, Rgba::WHITE, Vector2::ZERO, -Vector3::Z_AXIS, Vector3::X_AXIS, Vector3::Y_AXIS)
    , m_current_draw_instruction{}
{
    /* DO NOTHING */
}

MeshBuilder::MeshBuilder(const std::vector<Vertex3D>& verts, const std::vector<unsigned int> indcs)
    : verticies(verts)
    , indicies(indcs)
    , draw_instructions{}
    , vertex_prototype(Vector3::ZERO, Rgba::WHITE, Vector2::ZERO, -Vector3::Z_AXIS, Vector3::X_AXIS, Vector3::Y_AXIS)
    , m_current_draw_instruction{}
{
    /* DO NOTHING */
}

MeshBuilder::~MeshBuilder() {
    verticies.clear();
    indicies.clear();
    draw_instructions.clear();
}

void MeshBuilder::Begin(const PrimitiveType& type, bool hasIndexBuffer /*= true*/) {
    m_current_draw_instruction.type = type;
    m_current_draw_instruction.uses_index_buffer = hasIndexBuffer;
    m_current_draw_instruction.start_index = verticies.size();
}

void MeshBuilder::End() {
    m_current_draw_instruction.count = verticies.size() - m_current_draw_instruction.start_index;
    if(!draw_instructions.empty() && draw_instructions.back() == m_current_draw_instruction) {
        draw_instructions.back().count += m_current_draw_instruction.count;
    } else {
        draw_instructions.push_back(m_current_draw_instruction);
    }
}

void MeshBuilder::Clear() {
    verticies.clear();
    indicies.clear();
    draw_instructions.clear();
}

void MeshBuilder::SetTangent(const Vector3& tangent) {
    vertex_prototype.tangent = tangent;
}

void MeshBuilder::SetBitangent(const Vector3& bitangent) {
    vertex_prototype.bitangent = bitangent;
}

void MeshBuilder::SetNormal(const Vector3& normal) {
    vertex_prototype.normal = normal;
}

void MeshBuilder::SetColor(const Rgba& color) {
    vertex_prototype.color = color;
}

void MeshBuilder::SetUV(const Vector2& uv) {
    vertex_prototype.texCoords = uv;
}

void MeshBuilder::SetBoneWeights(const Vector4& bone_weights) {
    vertex_prototype.bone_weights = bone_weights;
}

void MeshBuilder::SetBoneIndices(const IntVector4& bone_indices) {
    vertex_prototype.bone_indices = bone_indices;
}

std::size_t MeshBuilder::AddVertex(const Vector3& position) {
    vertex_prototype.position = position;
    verticies.push_back(vertex_prototype);
    return verticies.size() - 1;
}

bool MeshBuilder::write(FileUtils::BinaryStream& stream) const {
    if(!stream.write(verticies.size())) {
        return false;
    }
    for(const auto & verticie : verticies) {
        if(!stream.write(verticie)) {
            return false;
        }
    }
    if(!stream.write(indicies.size())) {
        return false;
    }
    for(unsigned int indicie : indicies) {
        if(!stream.write(indicie)) {
            return false;
        }
    }
    if(!stream.write(draw_instructions.size())) {
        return false;
    }
    for(auto draw_instruction : draw_instructions) {
        if(!stream.write(static_cast<unsigned int>(draw_instruction.type))) {
            return false;
        }
        if(!stream.write(draw_instruction.start_index)) {
            return false;
        }
        if(!stream.write(draw_instruction.count)) {
            return false;
        }
        if(!stream.write(draw_instruction.uses_index_buffer)) {
            return false;
        }
    }
    return true;
}

bool MeshBuilder::read(FileUtils::BinaryStream& stream) {
    unsigned int v_s = 0;
    if(!stream.read(v_s)) {
        return false;
    }
    verticies.resize(v_s);
    for(auto & vertex : verticies) {
        if(!stream.read(vertex)) {
            return false;
        }
    }
    std::size_t i_s = 0;
    if(!stream.read(i_s)) {
        return false;
    }
    indicies.resize(i_s);
    for(unsigned int & index : indicies) {
        if(!stream.read(index)) {
            return false;
        }
    }
    std::size_t di_s = 0;
    if(!stream.read(di_s)) {
        return false;
    }
    draw_instructions.resize(di_s);
    for(auto & draw_instruction : draw_instructions) {
        draw_instruction_t cur_inst;
        if(!stream.read(cur_inst.type)) {
            return false;
        }
        if(!stream.read(cur_inst.start_index)) {
            return false;
        }
        if(!stream.read(cur_inst.count)) {
            return false;
        }
        if(!stream.read(cur_inst.uses_index_buffer)) {
            return false;
        }
        draw_instruction = cur_inst;
    }
    return true;
}