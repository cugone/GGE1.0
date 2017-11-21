#pragma once

#include <string>
#include <vector>

#include "Engine/Core/DataUtils.hpp"

class Shader;
class SimpleRenderer;
class RHIContext;
class Texture2D;
class TextureBase;

class Material {
public:
    Material(SimpleRenderer* renderer);
    Material(SimpleRenderer* renderer, const XMLElement& element);
    ~Material() = default;

    std::size_t GetTextureCount() const;

    const std::string& GetName() const;
    Texture2D* GetTexture(int index) const;
    Shader* GetShader() const;

    void AddTextureSlots(unsigned int count);
    void AddTextureSlot();

    void SetName(const std::string& name);
    void SetTexture(Texture2D* texture, int index = 0);
    void SetShader(Shader* shader);

protected:
    bool LoadFromXml(const XMLElement& element);
private:
    static unsigned int CUSTOM_TEXTURE_INDEX_OFFSET;
    static unsigned int MAX_CUSTOM_TEXTURE_COUNT;
    std::string _name;
    SimpleRenderer* _renderer;
    std::vector<Texture2D*> _textures;
    Shader* _shader;
};