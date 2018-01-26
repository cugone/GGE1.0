#include "Engine/Renderer/Mesh.hpp"

#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

Mesh::Mesh()
    : _local_transform()
    , _verts{}
    , _indicies{}
    , _positions{}
    , _uvs{}
    , _normals{}
    , _builder(new MeshBuilder())
    , _material(nullptr)
    , _draw_verts_only(false)
{
    /* DO NOTHING */
}

Mesh::Mesh(const std::vector<Vertex3D>& verts, const std::vector<unsigned int>& indicies)
    : _local_transform()
    , _verts(verts)
    , _indicies(indicies)
    , _positions{}
    , _uvs{}
    , _normals{}
    , _builder(new MeshBuilder(verts, indicies))
    , _material(nullptr)
    , _draw_verts_only(false)
{
    /* DO NOTHING */
}

Mesh::Mesh(const MeshBuilder& builder)
    : _local_transform()
    , _verts(builder.verticies)
    , _indicies(builder.indicies)
    , _positions{}
    , _uvs{}
    , _normals{}
    , _builder(new MeshBuilder(builder))
    , _material(nullptr)
    , _draw_verts_only(false)
{
    /* DO NOTHING */
}
Mesh::~Mesh() {
    delete _builder;
    _builder = nullptr;

    _verts.clear();
    _indicies.clear();
    _positions.clear();
    _uvs.clear();
    _normals.clear();

}
bool Mesh::LoadObjFromFile(const std::string& filepath) {
    return ParseObj(filepath);
}

void Mesh::Render(SimpleRenderer& renderer) const {
    renderer.RenderMesh(*this, _verts, _indicies);
}

void Mesh::RenderVertexesOnly(bool value) {
    _draw_verts_only = value;
}
bool Mesh::RenderVertexesOnly() const {
    return _draw_verts_only;
}

void Mesh::SetLocalTransform(const Matrix4& local_transform) {
    _local_transform = local_transform;
}
const Matrix4& Mesh::GetLocalTransform() const {
    return _local_transform;
}

bool Mesh::write(FileUtils::BinaryStream& stream) const {
    std::size_t p_s = _positions.size();
    for(std::size_t i = 0; i < p_s; ++i) {
        if(!stream.write(_positions[i])) {
            return false;
        }
    }
    std::size_t uv_s = _uvs.size();
    for(std::size_t i = 0; i < uv_s; ++i) {
        if(!stream.write(_uvs[i])) {
            return false;
        }
    }
    std::size_t n_s = _normals.size();
    for(std::size_t i = 0; i < n_s; ++i) {
        if(!stream.write(_normals[i])) {
            return false;
        }
    }
    return true;
}
bool Mesh::read(FileUtils::BinaryStream& stream) {
    std::size_t p_s = 0;
    if(!stream.read(p_s)) {
        return false;
    }
    _positions.resize(p_s);
    for(std::size_t i = 0; i < p_s; ++i) {
        if(!stream.read(_positions[i])) {
            return false;
        }
    }
    std::size_t uv_s = 0;
    if(!stream.read(uv_s)) {
        return false;
    }
    _uvs.resize(uv_s);
    for(std::size_t i = 0; i < uv_s; ++i) {
        if(!stream.read(_uvs[i])) {
            return false;
        }
    }
    std::size_t n_s = 0;
    if(!stream.read(n_s)) {
        return false;
    }
    _normals.resize(n_s);
    for(std::size_t i = 0; i < n_s; ++i) {
        if(!stream.read(_normals[i])) {
            return false;
        }
    }
    return true;
}

Material* Mesh::GetMaterial() const {
    return _material;
}

void Mesh::SetMaterial(Material* mat) {
    if(_material == mat) {
        return;
    }
    _material = mat;
}

MeshBuilder* Mesh::GetBuilder() const {
    return _builder;
}

bool Mesh::ParseObj(const std::string& filepath) {

    namespace FS = std::experimental::filesystem;

    FS::path path = filepath;
    if (FS::exists(path) == false) {
        return false;
    }

    std::stringstream ss;
    std::ifstream file_stream;
    file_stream.open(path);
    ss << file_stream.rdbuf();
    file_stream.close();

    unsigned int vertex_count = 0;
    unsigned int texture_coords_count = 0;
    unsigned int normal_count = 0;

    std::string cur_line;
    while (ss.good() && std::getline(ss, cur_line)) {
        
        std::string first_word;
        std::istringstream iss;
        iss.str(cur_line);
        iss >> first_word;
        if (first_word == "v") { //Vertex Positions
            ++vertex_count;
        }
        else if (first_word == "vt") { //Vertex Texture Coords (UVs)
            ++texture_coords_count;
        }
        else if (first_word == "vn") { //Vertex normals
            ++normal_count;
        }
    }

    _positions.reserve(vertex_count);
    _uvs.reserve(texture_coords_count);
    _normals.reserve(normal_count);

    ss.clear();
    ss.seekg(0);
    ss.seekp(0);

    unsigned long long line_count = 0;
    while (ss.good() && std::getline(ss, cur_line)) {
        ++line_count;
        std::vector<unsigned int> vertex_indexes;
        std::vector<unsigned int> uv_indexes;
        std::vector<unsigned int> normal_indexes;

        std::string first_word;
        std::istringstream iss;
        iss.str(cur_line);
        iss >> first_word;
        if (first_word == "v") { //Vertex Positions
            GetVertexes(cur_line, line_count);
        }
        if (first_word == "vt") {
            GetUvCoords(cur_line, line_count);
        }
        if (first_word == "vn") {
            GetNormals(cur_line, line_count);
        }
        if (first_word == "f") {
            GetIndicies(iss);
        }
    }

    _builder->verticies = _verts;
    _builder->indicies = _indicies;

    return true;
}
bool Mesh::GetVertexes(const std::string& cur_line, unsigned long long line_count) {
    std::string type;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
    std::istringstream ss;
    ss.str(cur_line);
    ss >> type; //consume tuple type.
    if (ss >> x) {
        if (ss >> y) {
            if (ss >> z) {
                if (ss >> w) {
                    x /= w;
                    y /= w;
                    z /= w;
                }
                Vector3 pos = Vector3(x, y, z);
                _positions.push_back(pos);
            } else {
                std::ostringstream ss_err;
                ss_err << "Malformed OBJ file: Line " << std::to_string(line_count) << ", Vertex " << std::to_string(line_count) << ": Parameter 3 is not a number or Only 2 parameters supplied.";
                ERROR_RECOVERABLE(ss_err.str().c_str());
                return false;
            }
        } else {
            std::ostringstream ss_err;
            ss_err << "Malformed OBJ file: Line " << std::to_string(line_count) << ", Vertex" << std::to_string(line_count) << ": Parameter 2 is not a number or Only 1 parameter supplied.";
            ERROR_RECOVERABLE(ss_err.str().c_str());
            return false;
        }
    } else {
        std::ostringstream ss_err;
        ss_err << "Malformed OBJ file: Line " << std::to_string(line_count) << ", Vertex " << std::to_string(line_count) << ": Parameter 1 is not a number.";
        ERROR_RECOVERABLE(ss_err.str().c_str());
        return false;
    }
    return true;
}
bool Mesh::GetUvCoords(const std::string& cur_line, unsigned long long line_count) {
    std::string type;
    float u = 0.0f;
    float v = 0.0f;
    float w = 1.0f;
    std::istringstream ss;
    ss.str(cur_line);
    ss >> type; //Consume type
    if (ss >> u) {
        if (ss >> v) {
            if (ss >> w) { //Consume W
                /* DO NOTHING */
            }
            Vector2 uv = Vector2(u, 1.0f - v); //DirectX specific
            _uvs.push_back(uv);
        } else {
            std::ostringstream ss_err;
            ss_err << "Malformed OBJ file: Line " << std::to_string(line_count) << ", TextureCoord " << std::to_string(line_count) << ": Parameter 2 is not a number.";
            ERROR_RECOVERABLE(ss_err.str().c_str());
            return false;
        }
    } else {
        std::ostringstream ss_err;
        ss_err << "Malformed OBJ file: Line " << std::to_string(line_count) << ", TextureCoord " << std::to_string(line_count) << ": Parameter 1 is not a number.";
        ERROR_RECOVERABLE(ss_err.str().c_str());
        return false;
    }
    return true;
}

bool Mesh::GetNormals(const std::string& cur_line, unsigned long long line_count) {
    std::string type;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    std::istringstream ss;
    ss.str(cur_line);
    ss >> type; //Consume type
    if (ss >> x) {
        if (ss >> y) {
            if (ss >> z) {
                Vector3 normal = Vector3(x, y, z);
                _normals.push_back(normal);
            } else {
                std::ostringstream ss_err;
                ss_err << "Malformed OBJ file: Line " << std::to_string(line_count) << ", Normal " << std::to_string(line_count) << ": Parameter 3 is not a number or only 2 parameters supplied.";
                ERROR_RECOVERABLE(ss_err.str().c_str());
                return false;
            }
        } else {
            std::ostringstream ss_err;
            ss_err << "Malformed OBJ file: Line " << std::to_string(line_count) << ", TextureCoord " << std::to_string(line_count) << ": Parameter 2 is not a number or only 1 parameter supplied.";
            ERROR_RECOVERABLE(ss_err.str().c_str());
            return false;
        }
    } else {
        std::ostringstream ss_err;
        ss_err << "Malformed OBJ file: Line " << std::to_string(line_count) << ", TextureCoord " << std::to_string(line_count) << ": Parameter 1 is not a number or no parameters supplied.";
        ERROR_RECOVERABLE(ss_err.str().c_str());
        return false;
    }
    return true;

}
bool Mesh::GetIndicies(std::istream& iss) {
    std::string cur_value;
    unsigned int value = 0;
    Vertex3D vert;
    std::size_t vertex_count = 0;
    unsigned int start_index = static_cast<unsigned int>(_verts.size());
    while (iss >> value) {
        //Required
        vert.position = _positions[value - 1];

        if (iss.peek() == '/') { //Do UVs and/or normals exist?
            iss.ignore();

            if (iss.peek() != '/') {
                if (iss >> value) {
                    vert.texCoords = _uvs[value - 1];
                }
            }

            if (iss.peek() == '/') {
                iss.ignore();

                if (iss >> value) {
                    vert.normal = _normals[value - 1];
                    iss.ignore();
                }
            }

        }
        vert.color = Rgba::WHITE;
        _verts.push_back(vert);

        ++vertex_count;
    }

    for (unsigned int i = 0; i < vertex_count - 2; ++i) {
        _indicies.push_back(start_index + 0);
        _indicies.push_back(start_index + (i + 2));
        _indicies.push_back(start_index + (i + 1));
    }
    return true;
}
