#include "Engine/Core/KerningFont.hpp"

#include <filesystem>
#include <map>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>
#include <utility>

#include "ThirdParty/TinyXML2/tinyxml2.h"

#include "Engine/core/ErrorWarningAssert.hpp"

KerningFont::KerningFont()
    : _image_paths{}
    , _filepath{}
    , _charmap()
    , _kernmap()
    , _info()
    , _common()
{
    /* DO NOTHING */
}

KerningFont::~KerningFont() {
    _image_paths.clear();
}

float KerningFont::CalculateTextHeight(const std::string& str, float scale /*= 1.0f*/) {
    return static_cast<float>((1 + std::count(str.begin(), str.end(), '\n')) * GetLineHeight() * scale);
}

float KerningFont::CalculateTextWidth(const std::string& text, float scale /*= 1.0f*/) {

    float cursor_x = 0;

    for(auto char_iter = text.begin(); char_iter != text.end(); /* DO NOTHING */) {
        KerningFont::CharDef current_charDef = GetCharDef(*char_iter);

        auto previous_char = char_iter++;
        float kern_value = 0.0f;
        if(char_iter != text.end()) {
            auto kern_iter = _kernmap.find(std::make_pair(*previous_char, *char_iter));
            if(kern_iter != _kernmap.end()) {
                kern_value = static_cast<float>(kern_iter->second);
            }
            cursor_x += (current_charDef.xadvance + kern_value);
        } else {
            KerningFont::CharDef previous_charDef = GetCharDef(*previous_char);
            cursor_x += (previous_charDef.xadvance);
        }
    }
    return cursor_x * scale;
}

int KerningFont::GetLineHeight() const {
    return _common.lineHeight;
}

float KerningFont::GetLineHeightAsUV() const {
    return static_cast<float>(_common.lineHeight) / static_cast<float>(_common.scaleH);
}

KerningFont::CharDef KerningFont::GetCharDef(int ch) const {
    auto chardef_iter = _charmap.find(ch);
    if(chardef_iter == _charmap.end()) {
        chardef_iter = _charmap.find(-1);
        if(chardef_iter == _charmap.end()) {
            return CharDef();
        }
        return chardef_iter->second;
    }
    return chardef_iter->second;
}

KerningFont::CommonDef KerningFont::GetCommonDef() const {
    return _common;
}

KerningFont::InfoDef KerningFont::GetInfoDef() const {
    return _info;
}

bool KerningFont::LoadFromFile(const std::string& filepath) {

    _filepath = filepath;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError doc_result = doc.LoadFile(filepath.c_str());
    if(doc_result != tinyxml2::XML_SUCCESS) {
        return false;
    }

    auto xml_root = doc.RootElement();
    if(xml_root == nullptr) {
        return false;
    }

    auto xml_info = xml_root->FirstChildElement("info");
    if(xml_info == nullptr) {
        return false;
    }

    /*<info
        face="Trebuchet MS"
        size="32"
        bold="0"
        italic="0"
        charset=""
        unicode="1"
        stretchH="100"
        smooth="1"
        aa="1"
        padding="0,0,0,0" up right down left
        spacing="1,1" horiz vert
        outline="0"
      />
    */

    _info.face = xml_info->Attribute("face");
    _info.em_size = xml_info->IntAttribute("size");
    _info.bold = xml_info->BoolAttribute("bold");
    _info.italic = xml_info->BoolAttribute("italic");
    _info.charset = xml_info->Attribute("charset");
    _info.unicode = xml_info->BoolAttribute("unicode");
    _info.stretchHeight = xml_info->FloatAttribute("stretchH") / 100.0f;
    _info.smoothing = xml_info->BoolAttribute("smooth");

    { //Temp variables scope constraint
        std::istringstream ss;
        ss.str(xml_info->Attribute("padding"));
        std::string temp_string;
        int temp = 0;
        char delim = ',';
        while(std::getline(ss, temp_string, delim)) {
            switch(temp) {
                case 0:
                    _info.padding_up = std::stoi(temp_string);
                    break;
                case 1:
                    _info.padding_right = std::stoi(temp_string);
                    break;
                case 2:
                    _info.padding_down = std::stoi(temp_string);
                    break;
                case 3:
                    _info.padding_left = std::stoi(temp_string);
                    break;
                default:
                    ERROR_AND_DIE("FONT FORMAT INFO PADDING CHANGED");
            }
            ++temp;
        }
    }

    { //Temp variables scope constraint
        std::istringstream ss;
        ss.str(xml_info->Attribute("spacing"));
        std::string temp_string;
        int temp = 0;
        char delim = ',';
        while(std::getline(ss, temp_string, delim)) {
            switch(temp) {
                case 0:
                    _info.spacing_horizontal = std::stoi(temp_string);
                    break;
                case 1:
                    _info.spacing_vertical = std::stoi(temp_string);
                    break;
                default:
                    ERROR_AND_DIE("FONT FORMAT INFO SPACING CHANGED");
            }
            ++temp;
        }
    }

    _info.outline = xml_info->BoolAttribute("outline");
    

    auto xml_common = xml_root->FirstChildElement("common");
    if(xml_common == nullptr) {
        return false;
    }

    /*<common
        lineHeight="32"
        base="25"
        scaleW="512"
        scaleH="512"
        pages="1"
        packed="0"
        alphaChnl="0"
        redChnl="4"
        greenChnl="4"
        blueChnl="4"
      />

    */

    _common.lineHeight = xml_common->IntAttribute("lineHeight");
    _common.base = xml_common->IntAttribute("base");
    _common.scaleW = xml_common->IntAttribute("scaleW");
    _common.scaleH = xml_common->IntAttribute("scaleH");
    _common.pages = xml_common->IntAttribute("pages");
    _common.packed = xml_common->BoolAttribute("packed");
    _common.alpha_channel = xml_common->IntAttribute("alphaChnl");
    _common.red_channel = xml_common->IntAttribute("redChnl");
    _common.green_channel = xml_common->IntAttribute("greenChnl");
    _common.blue_channel = xml_common->IntAttribute("blueChnl");

    auto xml_pages = xml_root->FirstChildElement("pages");
    if(xml_pages == nullptr) {
        return false;
    }

    {//Scope constraint
        namespace FS = std::experimental::filesystem;
        FS::path p("Data/Fonts/");
        for(auto xml_page = xml_pages->FirstChildElement("page"); xml_page != nullptr; xml_page = xml_page->NextSiblingElement("page")) {
            unsigned int page_id = xml_page->UnsignedAttribute("id");
            page_id; //UNUSED
            FS::path page_file = xml_page->Attribute("file");
            p += page_file;
            _image_paths.push_back(p.string());
        }
    }

    auto xml_chars = xml_root->FirstChildElement("chars");
    if(xml_chars == nullptr) {
        return false;
    }
    unsigned int char_count = xml_chars->UnsignedAttribute("count");
    char_count; //UNUSED

    for(auto xml_char = xml_chars->FirstChildElement("char"); xml_char != nullptr; xml_char = xml_char->NextSiblingElement("char")) {
        int id = xml_char->IntAttribute("id");
        CharDef t;
        t.x = xml_char->IntAttribute("x");
        t.y = xml_char->IntAttribute("y");
        t.width = xml_char->IntAttribute("width");
        t.height = xml_char->IntAttribute("height");
        t.xoffset = xml_char->IntAttribute("xoffset");
        t.yoffset = xml_char->IntAttribute("yoffset");
        t.xadvance = xml_char->IntAttribute("xadvance");
        t.page = xml_char->IntAttribute("page");
        t.channel = xml_char->IntAttribute("chnl");

        _charmap.insert_or_assign(id, t);
    }

    auto xml_kernings = xml_root->FirstChildElement("kernings");
    if(xml_kernings) {
        for(auto xml_kern = xml_kernings->FirstChildElement("kerning"); xml_kern != nullptr; xml_kern = xml_kern->NextSiblingElement("kerning")) {
            int first = xml_kern->IntAttribute("first");
            int second = xml_kern->IntAttribute("second");
            int amount = xml_kern->IntAttribute("amount");

            _kernmap.insert_or_assign(std::make_pair(first, second), amount);
        }
    }


    return true;
}
