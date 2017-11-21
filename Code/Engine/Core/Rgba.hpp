#pragma once

#include <string>
#include <vector>

class Rgba {
public:

    static const Rgba WHITE;
    static const Rgba BLACK;
    static const Rgba RED;
    static const Rgba GREEN;
    static const Rgba FORESTGREEN;
    static const Rgba BLUE;
    static const Rgba NAVY_BLUE;
    static const Rgba CYAN;
    static const Rgba YELLOW;
    static const Rgba MAGENTA;
    static const Rgba ORANGE;
    static const Rgba VIOLET;
    static const Rgba LIGHT_GREY;
    static const Rgba LIGHT_GRAY;
    static const Rgba GREY;
    static const Rgba GRAY;
    static const Rgba DARK_GREY;
    static const Rgba DARK_GRAY;
    static const Rgba OLIVE;
    static const Rgba SKY_BLUE;
    static const Rgba LIME;
    static const Rgba TEAL;
    static const Rgba TURQUOISE;
    static const Rgba PERIWINKLE;
    static const Rgba NORMAL_Z;
    static const Rgba NOALPHA;

    static Rgba Random();
    static Rgba RandomWithAlpha();

    Rgba();
    Rgba(const Rgba& copy);
    Rgba(Rgba&& r_copy) = default;
    Rgba& operator=(const Rgba& rhs) = default;
    Rgba& operator=(Rgba&& r_rhs) = default;
    ~Rgba() = default;

    explicit Rgba(std::string name);
    explicit Rgba(const char* name);
    explicit Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0xFF);

    void SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255);
    void SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha = 1.0f);
    void GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const;
    void ScaleRGB(float rgbScale);
    void ScaleAlpha(float alphaScale);

    unsigned int GetAsRawValue() const;

    bool operator==(const Rgba& rhs) const;
    bool operator!=(const Rgba& rhs) const;

    bool IsRgbEqual(const Rgba& rhs) const;

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;


    friend Rgba Interpolate(const Rgba& a, const Rgba& b, float t);
    
private:
    void SetValueFromName(std::string name);
};
