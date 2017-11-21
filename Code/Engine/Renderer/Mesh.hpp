#pragma once

#include <string>
#include <vector>

#include "Engine/Core/FileUtils.hpp"

#include "Engine/Renderer/Vertex3D.hpp"

#include "Engine/Math/Matrix4.hpp"

class SimpleRenderer;
class MeshBuilder;
class Material;

class Mesh {
public:
	Mesh();
    Mesh(const std::vector<Vertex3D>& verts, const std::vector<unsigned int>& indicies);
    explicit Mesh(const MeshBuilder& builder);
	~Mesh();

    bool LoadObjFromFile(const std::string& filepath);

    void Render(SimpleRenderer& renderer) const;

    void RenderVertexesOnly(bool value);
    bool RenderVertexesOnly() const;

    void SetLocalTransform(const Matrix4& local_transform);
    const Matrix4& GetLocalTransform() const;


    bool write(FileUtils::BinaryStream& stream) const;
    bool read(FileUtils::BinaryStream& stream);

    Material* GetMaterial() const;
    void SetMaterial(Material* mat);

    MeshBuilder* GetBuilder() const;

protected:
private:
    Matrix4 _local_transform;
    std::vector<Vertex3D> _verts;
    std::vector<unsigned int> _indicies;
    std::vector<Vector3> _positions;
    std::vector<Vector2> _uvs;
    std::vector<Vector3> _normals;
    MeshBuilder* _builder;
    Material* _material;
    bool _draw_verts_only;

    bool ParseObj(const std::string& filepath);
    bool GetVertexes(const std::string& cur_line, unsigned long long line_count);
    bool GetUvCoords(const std::string& cur_line, unsigned long long line_count);
    bool GetNormals(const std::string& cur_line, unsigned long long line_count);
    bool GetIndicies(std::istream& iss);
};