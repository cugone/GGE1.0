#include "Engine/Core/Rgba.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Math/MathUtils.hpp"

const Rgba Rgba::WHITE = Rgba(255, 255, 255, 255);
const Rgba Rgba::BLACK = Rgba(0, 0, 0, 255);
const Rgba Rgba::RED = Rgba(255, 0, 0, 255);
const Rgba Rgba::GREEN = Rgba(0, 255, 0, 255);
const Rgba Rgba::FORESTGREEN = Rgba(34, 139, 34, 255);
const Rgba Rgba::BLUE = Rgba(0, 0, 255, 255);
const Rgba Rgba::NAVY_BLUE = Rgba(0, 0, 128, 255);
const Rgba Rgba::CYAN = Rgba(0, 255, 255, 255);
const Rgba Rgba::YELLOW = Rgba(255, 255, 0, 255);
const Rgba Rgba::MAGENTA = Rgba(255, 0, 255, 255);
const Rgba Rgba::ORANGE = Rgba(255, 165, 0, 255);
const Rgba Rgba::VIOLET = Rgba(143, 0, 255, 255);
const Rgba Rgba::GREY = Rgba(128, 128, 128, 255);
const Rgba Rgba::GRAY = Rgba(128, 128, 128, 255);
const Rgba Rgba::LIGHT_GREY = Rgba(192, 192, 192, 255);
const Rgba Rgba::LIGHT_GRAY = Rgba(192, 192, 192, 255);
const Rgba Rgba::DARK_GREY = Rgba(64, 64, 64, 255);
const Rgba Rgba::DARK_GRAY = Rgba(64, 64, 64, 255);
const Rgba Rgba::OLIVE = Rgba(107, 142, 35, 255);
const Rgba Rgba::SKY_BLUE = Rgba(45, 185, 255, 255);
const Rgba Rgba::LIME = Rgba(227, 255, 0, 255);
const Rgba Rgba::TEAL = Rgba(0, 128, 128, 255);
const Rgba Rgba::TURQUOISE = Rgba(64, 224, 208, 255);
const Rgba Rgba::PERIWINKLE = Rgba(204, 204, 255, 255);
const Rgba Rgba::NORMAL_Z = Rgba(128, 128, 255, 255);
const Rgba Rgba::NOALPHA = Rgba(0, 0, 0, 0);

Rgba Rgba::Random() {
    return Rgba(static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256)),
                static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256)),
                static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256)),
                255);
}

Rgba Rgba::RandomWithAlpha() {
    return Rgba(static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256)),
                static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256)),
                static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256)),
                static_cast<unsigned char>(MathUtils::GetRandomIntLessThan(256)));
}

Rgba::Rgba()
: r(255)
, g(255)
, b(255)
, a(255)
{
    /* DO NOTHING */
}

Rgba::Rgba(const Rgba& copy)
: r(copy.r)
, g(copy.g)
, b(copy.b)
, a(copy.a)
{
    /* DO NOTHING */
}

Rgba::Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha /*= 0xFF*/)
: r(red)
, g(green)
, b(blue)
, a(alpha)
{
    /* DO NOTHING */
}

Rgba::Rgba(std::string name)
    : r(255)
    , g(255)
    , b(255)
    , a(255)
{
    name = ToUpperCase(name);

    std::size_t hash_loc = name.find_first_of('#');
    if(hash_loc != std::string::npos) {
        name.replace(hash_loc, 1, "0X");
        std::size_t char_count = 0;
        unsigned long value_int = std::stoul(name, &char_count, 16);
        if(char_count == 10) { //10 characters counted: 0xRRGGBBAA = 10 chars
            r = static_cast<unsigned char>((value_int & 0xff000000ul) >> 24);
            g = static_cast<unsigned char>((value_int & 0x00ff0000ul) >> 16);
            b = static_cast<unsigned char>((value_int & 0x0000ff00ul) >> 8);
            a = static_cast<unsigned char>((value_int & 0x000000fful) >> 0);
        } else if(char_count == 8) { //8 characters counted: 0xRRGGBB = 8 chars
            r = static_cast<unsigned char>((value_int & 0xff0000ul) >> 16);
            g = static_cast<unsigned char>((value_int & 0x00ff00ul) >> 8);
            b = static_cast<unsigned char>((value_int & 0x0000fful) >> 0);
            a = 255;
        } else {
            ERROR_AND_DIE("ill-formed Rgba from string. Hexidecimal values must start with hash (#) and require two-digits per color plus optional alpha: #RRGGBB[AA]");
        }
    } else {
        std::istringstream ss(name);
        std::vector<std::string> v;
        for(std::string color_components; std::getline(ss, color_components, ','); /* DO NOTHING */ ) {
            v.push_back(color_components);
        }
        std::size_t v_s = v.size();
        if(v_s > 1) {
            if(v_s < 3) {
                ERROR_AND_DIE("ill-formed Rgba from string. Color components must have at least 3 colors from 0-255.");
            }
            r = static_cast<unsigned char>(std::stoi(v[0].data()));
            g = static_cast<unsigned char>(std::stoi(v[1].data()));
            b = static_cast<unsigned char>(std::stoi(v[2].data()));
            if(v_s > 3) {
                a = static_cast<unsigned char>(std::stoi(v[3].data()));
            }
        } else {
            SetValueFromName(v[0]);
        }
    }
}

Rgba::Rgba(const char* name)
    : Rgba(std::string(name ? name : ""))
{
    /* DO NOTHING */
}

void Rgba::SetValueFromName(std::string name) {

    std::string nameUpper = ToUpperCase(name);

    if(nameUpper == "WHITE") {
        r = WHITE.r;
        g = WHITE.g;
        b = WHITE.b;
        a = WHITE.a;
    } else if(nameUpper == "BLACK") {
        r = BLACK.r;
        g = BLACK.g;
        b = BLACK.b;
        a = BLACK.a;
    } else if(nameUpper == "RED") {
        r = RED.r;
        g = RED.g;
        b = RED.b;
        a = RED.a;
    } else if(nameUpper == "GREEN") {
        r = GREEN.r;
        g = GREEN.g;
        b = GREEN.b;
        a = GREEN.a;
    } else if(nameUpper == "FORESTGREEN") {
        r = FORESTGREEN.r;
        g = FORESTGREEN.g;
        b = FORESTGREEN.b;
        a = FORESTGREEN.a;
    } else if(nameUpper == "BLUE") {
        r = BLUE.r;
        g = BLUE.g;
        b = BLUE.b;
        a = BLUE.a;
    } else if(nameUpper == "NAVYBLUE") {
        r = NAVY_BLUE.r;
        g = NAVY_BLUE.g;
        b = NAVY_BLUE.b;
        a = NAVY_BLUE.a;
    } else if(nameUpper == "CYAN") {
        r = CYAN.r;
        g = CYAN.g;
        b = CYAN.b;
        a = CYAN.a;
    } else if(nameUpper == "YELLOW") {
        r = YELLOW.r;
        g = YELLOW.g;
        b = YELLOW.b;
        a = YELLOW.a;
    } else if(nameUpper == "MAGENTA") {
        r = MAGENTA.r;
        g = MAGENTA.g;
        b = MAGENTA.b;
        a = MAGENTA.a;
    } else if(nameUpper == "ORANGE") {
        r = ORANGE.r;
        g = ORANGE.g;
        b = ORANGE.b;
        a = ORANGE.a;
    } else if(nameUpper == "VIOLET") {
        r = VIOLET.r;
        g = VIOLET.g;
        b = VIOLET.b;
        a = VIOLET.a;
    } else if(nameUpper == "GREY") {
        r = GREY.r;
        g = GREY.g;
        b = GREY.b;
        a = GREY.a;
    } else if(nameUpper == "GRAY") {
        r = GRAY.r;
        g = GRAY.g;
        b = GRAY.b;
        a = GRAY.a;
    } else if(nameUpper == "LIGHTGREY") {
        r = LIGHT_GREY.r;
        g = LIGHT_GREY.g;
        b = LIGHT_GREY.b;
        a = LIGHT_GREY.a;
    } else if(nameUpper == "LIGHTGRAY") {
        r = LIGHT_GRAY.r;
        g = LIGHT_GRAY.g;
        b = LIGHT_GRAY.b;
        a = LIGHT_GRAY.a;
    } else if(nameUpper == "DARKGREY") {
        r = DARK_GREY.r;
        g = DARK_GREY.g;
        b = DARK_GREY.b;
        a = DARK_GREY.a;
    } else if(nameUpper == "DARKGRAY") {
        r = DARK_GRAY.r;
        g = DARK_GRAY.g;
        b = DARK_GRAY.b;
        a = DARK_GRAY.a;
    } else if(nameUpper == "OLIVE") {
        r = OLIVE.r;
        g = OLIVE.g;
        b = OLIVE.b;
        a = OLIVE.a;
    } else if(nameUpper == "SKYBLUE") {
        r = SKY_BLUE.r;
        g = SKY_BLUE.g;
        b = SKY_BLUE.b;
        a = SKY_BLUE.a;
    } else if(nameUpper == "LIME") {
        r = LIME.r;
        g = LIME.g;
        b = LIME.b;
        a = LIME.a;
    } else if(nameUpper == "TEAL") {
        r = TEAL.r;
        g = TEAL.g;
        b = TEAL.b;
        a = TEAL.a;
    } else if(nameUpper == "TURQUOISE") {
        r = TURQUOISE.r;
        g = TURQUOISE.g;
        b = TURQUOISE.b;
        a = TURQUOISE.a;
    } else if(nameUpper == "PERIWINKLE") {
        r = PERIWINKLE.r;
        g = PERIWINKLE.g;
        b = PERIWINKLE.b;
        a = PERIWINKLE.a;
    } else if(nameUpper == "NORMALZ") {
        r = NORMAL_Z.r;
        g = NORMAL_Z.g;
        b = NORMAL_Z.b;
        a = NORMAL_Z.a;
    } else if(nameUpper == "NOALPHA") {
        r = NOALPHA.r;
        g = NOALPHA.g;
        b = NOALPHA.b;
        a = NOALPHA.a;
    } else {
        r = WHITE.r;
        g = WHITE.g;
        b = WHITE.b;
        a = WHITE.a;
    }
}

void Rgba::SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte /*= 255*/) {
    r = redByte;
    g = greenByte;
    b = blueByte;
    a = alphaByte;
}

void Rgba::SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha /*= 1.0f*/) {
    r = static_cast<unsigned char>(normalizedRed) * 255;
    g = static_cast<unsigned char>(normalizedGreen) * 255;
    b = static_cast<unsigned char>(normalizedBlue) * 255;
    a = static_cast<unsigned char>(normalizedAlpha) * 255;
}

void Rgba::GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const {
    out_normalizedRed = r / 255.0f;
    out_normalizedGreen = g / 255.0f;
    out_normalizedBlue = b / 255.0f;
    out_normalizedAlpha = a / 255.0f;
}

void Rgba::ScaleRGB(float rgbScale) {
    float scaledR = static_cast<float>(r) * rgbScale;
    float scaledG = static_cast<float>(g) * rgbScale;
    float scaledB = static_cast<float>(b) * rgbScale;

    r = static_cast<unsigned char>(MathUtils::Clamp<float>(scaledR, 0.0f, 255.0f));
    g = static_cast<unsigned char>(MathUtils::Clamp<float>(scaledG, 0.0f, 255.0f));
    b = static_cast<unsigned char>(MathUtils::Clamp<float>(scaledB, 0.0f, 255.0f));
}

void Rgba::ScaleAlpha(float alphaScale) {
    float scaledA = static_cast<float>(a) * alphaScale;

    a = static_cast<unsigned char>(MathUtils::Clamp<float>(scaledA, 0.0f, 255.0f));
}

unsigned int Rgba::GetAsRawValue() const {
    return static_cast<unsigned int>(((static_cast<unsigned int>(r) << 24) & 0xFF000000) | ((static_cast<unsigned int>(g) << 16) & 0x00FF0000) | ((static_cast<unsigned int>(b) << 8) & 0x0000FF00) | ((static_cast<unsigned int>(a) << 0) & 0x000000FF));
}
bool Rgba::IsRgbEqual(const Rgba& rhs) const {
    return r == rhs.r && g == rhs.g && b == rhs.b;
}

Rgba Interpolate(const Rgba& a, const Rgba& b, float t) {
    float rA = a.r;
    float gA = a.g;
    float bA = a.b;
    float aA = a.a;

    float rB = b.r;
    float gB = b.g;
    float bB = b.b;
    float aB = b.a;

    unsigned char rR = static_cast<unsigned char>(((1.0f - t) * rA) + (t * rB));
    unsigned char gR = static_cast<unsigned char>(((1.0f - t) * gA) + (t * gB));
    unsigned char bR = static_cast<unsigned char>(((1.0f - t) * bA) + (t * bB));
    unsigned char aR = static_cast<unsigned char>(((1.0f - t) * aA) + (t * aB));

    return Rgba(rR, gR, bR, aR);
}

bool Rgba::operator==(const Rgba& rhs) const {
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
}

bool Rgba::operator!=(const Rgba& rhs) const {
    return !(*this == rhs);
}
