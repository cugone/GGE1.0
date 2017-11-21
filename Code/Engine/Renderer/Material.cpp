#include "Engine/Renderer/Material.hpp"

#include <filesystem>

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Renderer/SimpleRenderer.hpp"
#include "Engine/Renderer/Texture2D.hpp"

unsigned int Material::CUSTOM_TEXTURE_INDEX_OFFSET = 6;
unsigned int Material::MAX_CUSTOM_TEXTURE_COUNT = 58;

Material::Material(SimpleRenderer* renderer)
: _name("UNNAMED_MATERIAL")
, _renderer(renderer)
, _textures(CUSTOM_TEXTURE_INDEX_OFFSET, nullptr)
, _shader(nullptr)
{

    _textures[0] = _renderer->GetTexture("__diffuse");
    _textures[1] = _renderer->GetTexture("__normal");
    _textures[2] = _renderer->GetTexture("__lighting");
    _textures[3] = _renderer->GetTexture("__specular");
    _textures[4] = _renderer->GetTexture("__occlusion");
    _textures[5] = _renderer->GetTexture("__emissive");

}

Material::Material(SimpleRenderer* renderer, const XMLElement& element)
: _name{}
, _renderer(renderer)
, _textures(CUSTOM_TEXTURE_INDEX_OFFSET, nullptr)
, _shader(nullptr)
{

    _textures[0] = _renderer->GetTexture("__diffuse");
    _textures[1] = _renderer->GetTexture("__normal");
    _textures[2] = _renderer->GetTexture("__lighting");
    _textures[3] = _renderer->GetTexture("__specular");
    _textures[4] = _renderer->GetTexture("__occlusion");
    _textures[5] = _renderer->GetTexture("__emissive");

    LoadFromXml(element);
}

std::size_t Material::GetTextureCount() const {
    return _textures.size();
}

const std::string& Material::GetName() const {
    return _name;
}

Texture2D* Material::GetTexture(int index) const {
    return _textures[index];
}

Shader* Material::GetShader() const {
    return _shader;
}

void Material::AddTextureSlots(unsigned int count) {
    std::size_t old_size = _textures.size();
    std::size_t new_size = old_size + (std::min)(MAX_CUSTOM_TEXTURE_COUNT, count);
    _textures.resize(new_size);
    for(std::size_t i = old_size; i < new_size; ++i) {
        _textures[i] = nullptr;
    }
}

void Material::AddTextureSlot() {
    AddTextureSlots(1);
}

void Material::SetName(const std::string& name) {
    _name = name;
}
void Material::SetTexture(Texture2D* texture, int index /*= 0*/) {
    _textures[index] = texture;
}

void Material::SetShader(Shader* shader) {
    _shader = shader;
}

bool Material::LoadFromXml(const XMLElement& element) {
    namespace FS = std::experimental::filesystem;

    auto nameAsCstr = element.Name();
    bool is_material = std::string(nameAsCstr ? nameAsCstr : "") == "material";
    if(!is_material) {
        return false;
    }

    DataUtils::ValidateXmlElement(element, "", "name", "shader,textures");

    _name = DataUtils::ParseXmlAttribute(element, std::string("name"), std::string(""));

    auto xml_shader = element.FirstChildElement("shader");
    if(xml_shader != nullptr) {
        DataUtils::ValidateXmlElement(*xml_shader, "", "src");
        auto file = DataUtils::ParseXmlAttribute(*xml_shader, "src", "");
        FS::path p(file);
        auto shader = _renderer->GetShader(p.string());
        if(shader == nullptr) {
            ERROR_AND_DIE("Shader referenced in Material file does not already exist.");
        }
        _shader = shader;
    }

    auto xml_Textures = element.FirstChildElement("textures");
    auto loaded_textures = _renderer->GetLoadedTextures();
    auto invalid_tex = _renderer->GetTexture("__invalid");
    if(xml_Textures != nullptr) {

        auto xml_diffuse  = xml_Textures->FirstChildElement("diffuse");
        if(xml_diffuse) {
            auto file = DataUtils::ParseXmlAttribute(*xml_diffuse, "src", "");
            FS::path p(file);
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[0] = tex;
        }

        auto xml_normal = xml_Textures->FirstChildElement("normal");
        if(xml_normal) {
            auto file = DataUtils::ParseXmlAttribute(*xml_normal, "src", "");
            FS::path p(file);
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[1] = tex;
        }

        auto xml_lighting = xml_Textures->FirstChildElement("lighting");
        if(xml_lighting) {
            auto file = DataUtils::ParseXmlAttribute(*xml_lighting, "src", "");
            FS::path p(file);
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[2] = tex;
        }

        auto xml_specular = xml_Textures->FirstChildElement("specular");
        if(xml_specular) {
            auto file = DataUtils::ParseXmlAttribute(*xml_specular, "src", "");
            FS::path p(file);
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[3] = tex;
        }

        auto xml_occlusion = xml_Textures->FirstChildElement("occlusion");
        if(xml_occlusion) {
            auto file = DataUtils::ParseXmlAttribute(*xml_occlusion, "src", "");
            FS::path p(file);
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[4] = tex;
        }

        auto xml_emissive = xml_Textures->FirstChildElement("emissive");
        if(xml_emissive) {
            auto file = DataUtils::ParseXmlAttribute(*xml_emissive, "src", "");
            FS::path p(file);
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[5] = tex;
        }

        auto numTextures = DataUtils::GetChildElementCount(*xml_Textures, "texture");
        if(numTextures >= MAX_CUSTOM_TEXTURE_COUNT) {
            g_theFileLogger->LogWarnf("Max custom texture count exceeded. Cannot bind more than %i custom textures.", MAX_CUSTOM_TEXTURE_COUNT);
        }
        AddTextureSlots(numTextures);
        for(auto xml_texture = xml_Textures->FirstChildElement("texture");
            xml_texture != nullptr;
            xml_texture = xml_texture->NextSiblingElement("texture"))
        {
            DataUtils::ValidateXmlElement(*xml_texture, "", "index,src");
            std::size_t index = CUSTOM_TEXTURE_INDEX_OFFSET + DataUtils::ParseXmlAttribute(*xml_texture, std::string("index"), 0u);
            if(index >= MAX_CUSTOM_TEXTURE_COUNT) {
                continue;
            }
            auto file = DataUtils::ParseXmlAttribute(*xml_texture, "src", "");
            FS::path p(file);
            const auto& p_str = p.string();
            bool empty_path = p.empty();
            bool texture_not_exist = !empty_path && loaded_textures.find(p_str) == loaded_textures.end();
            bool invalid_src = empty_path || texture_not_exist;
            auto tex = invalid_src ? invalid_tex : (_renderer->GetTexture(p_str));
            _textures[index] = tex;
        }
    }
    return true;
}
