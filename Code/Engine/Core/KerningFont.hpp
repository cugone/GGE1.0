#pragma once

#include <map>
#include <string>
#include <vector>

class KerningFont {
public:
    struct InfoDef {
        std::string face;        //name of true type font
        int em_size;             //size of font in em
        bool bold;               //is bold
        bool italic;             //is italic
        std::string charset;     //OEM charset if not unicode (otherwise empty)
        bool unicode;            //is unicode
        float stretchHeight;     //font height stretch percentage
        bool smoothing;          //is smoothed
        int antialiased;         //supersamping level 1 for none
        int padding_up;          //top padding 
        int padding_right;       //right side padding
        int padding_down;        //bottom padding
        int padding_left;        //left side padding
        int spacing_horizontal;  //horizontal spacing
        int spacing_vertical;    //vertical spacing
        int outline;             //outline thickness
    };
    struct CommonDef {
        int lineHeight;    //Distance in pixels between each line
        int base;          //Number of pixels from the absolute top of the line to the base of the characters.
        int scaleW;        //The width of the texture, normally used to scale the x pos of the character image.
        int scaleH;        //The height of the texture, normally used to scale the y pos of the character image.
        int pages;         //The number of texture pages included in the font.
        bool packed;       //true if monochrome characters have been packed into each texture channel. Look to alpha_channel for what is stored.
        int alpha_channel; //0 -- channel holds glyph data;
                           //1 -- channel holds outline;
                           //2 -- channel holds glyph and outline;
                           //3 -- channel set to zero;
                           //4 -- channel set to one.
        int red_channel;
        int green_channel;
        int blue_channel;
    };

    struct CharDef {
        int x;         //x pixel position in the texture for quad generation
        int y;         //y pixel position in the texture for quad generation
        int width;     //width in pixels of the glyph for quad generation
        int height;    //height in pixels of the glyph for quad generation
        int xoffset;   //cursor position x offset
        int yoffset;   //cursor position y offset
        int xadvance;  //how far to move the cursor after drawing
        int page;      //page id where the glyph is located.
        int channel;   //channel
        CharDef()
            : x(0)
            , y(0)
            , width(0)
            , height(0)
            , xoffset(0)
            , yoffset(0)
            , xadvance(0)
            , page(0)
            , channel(0) {
            /* DO NOTHING */
        }
    };

	KerningFont();
	~KerningFont();

    float CalculateTextHeight(const std::string& text, float scale = 1.0f);
    float CalculateTextWidth(const std::string& text, float scale = 1.0f);
    int GetLineHeight() const;
    float GetLineHeightAsUV() const;
    CharDef GetCharDef(int ch) const;
    CommonDef GetCommonDef() const;
    InfoDef GetInfoDef() const;
protected:
private:
    using CharMap = std::map<int, CharDef>;
    using KerningMap = std::map<std::pair<int, int>, int>;
    bool LoadFromFile(const std::string& filepath);
    std::vector<std::string> _image_paths;
    std::string _filepath;
    CharMap _charmap;
    KerningMap _kernmap;
    InfoDef _info;
    CommonDef _common;


    friend class SimpleRenderer;
};