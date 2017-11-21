#include "Engine/Core/Base64.hpp"

#include <algorithm>

namespace DataUtils {

const std::vector<char> Base64Encoder::EncodingTable = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z', //0-25
'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z', //26-51
'0','1','2','3','4','5','6','7','8','9',                                                                 //52-61
'+','/' };

const char Base64Encoder::PADDING_CHAR = '=';

/************************************************************************/
/* ENCODER                                                              */
/************************************************************************/

int Base64Encoder::GetFirstSymbolIndex(const std::vector<unsigned char>& encoding_buffer) const noexcept {
    return ((encoding_buffer[0] & 0xFC) >> 2);
}

int Base64Encoder::GetSecondSymbolIndex(const std::vector<unsigned char>& encoding_buffer) const noexcept {
    return (((encoding_buffer[0] & 0x03) << 4) | ((encoding_buffer[1] & 0xF0) >> 4));
}

int Base64Encoder::GetThirdSymbolIndex(const std::vector<unsigned char>& encoding_buffer) const noexcept {
    return (((encoding_buffer[1] & 0x0F) << 2) | ((encoding_buffer[2] & 0xC0) >> 6));
}

int Base64Encoder::GetFourthSymbolIndex(const std::vector<unsigned char>& encoding_buffer) const noexcept {
    return (encoding_buffer[2] & 0x3F);
}

//Gets the 6 most significant digits of the first byte.
unsigned char Base64Encoder::GetFirstSymbol(const std::vector<unsigned char>& encoding_buffer) const noexcept {
    return Base64Encoder::EncodingTable[Base64Encoder::GetFirstSymbolIndex(encoding_buffer)];
}

//Gets the 2 least significant digits from previous (first) byte and 4 most significant from the second byte.
unsigned char Base64Encoder::GetSecondSymbol(const std::vector<unsigned char>& encoding_buffer) const noexcept {
    return Base64Encoder::EncodingTable[Base64Encoder::GetSecondSymbolIndex(encoding_buffer)];
}

//Gets the 4 least significant digits from previous (second) byte and 2 least significant from the third byte.
unsigned char Base64Encoder::GetThirdSymbol(const std::vector<unsigned char>& encoding_buffer) const noexcept {
    return Base64Encoder::EncodingTable[Base64Encoder::GetThirdSymbolIndex(encoding_buffer)];
}

//Gets the 6 least significant digits from the third byte.
unsigned char Base64Encoder::GetFourthSymbol(const std::vector<unsigned char>& encoding_buffer) const noexcept {
    return Base64Encoder::EncodingTable[Base64Encoder::GetFourthSymbolIndex(encoding_buffer)];
}

std::string Base64Encoder::Encode(std::ifstream& file_input_stream) const noexcept {
    if(!file_input_stream.is_open() || file_input_stream.fail()) {
        return std::string{};
    }
    return Encode(static_cast<std::istream&>(file_input_stream));
}

std::string Base64Encoder::Encode(std::istream& input_stream) const noexcept {

    if(input_stream.fail()) {
        return std::string{};
    }

    unsigned long file_size = 0;
    std::vector<unsigned char> encoding_buffer = { '\0', '\0', '\0' };

    input_stream.seekg(0, std::ios::end);
    file_size = static_cast<unsigned long>(input_stream.tellg());
    input_stream.seekg(0);

    std::string output(static_cast<unsigned long>(4 * std::ceil(file_size / 3.0)), '\0');

    if(file_size == 0) {
        output.clear();
        input_stream.clear();
        return output;
    }

    while(input_stream.read(reinterpret_cast<char*>(encoding_buffer.data()), encoding_buffer.size())) {

        char firstsymbol = GetFirstSymbol(encoding_buffer);
        char secondsymbol = GetSecondSymbol(encoding_buffer);
        char thirdsymbol = GetThirdSymbol(encoding_buffer);
        char fourthsymbol = GetFourthSymbol(encoding_buffer);

        unsigned long s = 4 * static_cast<unsigned long>(input_stream.tellg()) / 3;

        output[s - 4] = firstsymbol;
        output[s - 3] = secondsymbol;
        output[s - 2] = thirdsymbol;
        output[s - 1] = fourthsymbol;

    }

    output.erase(output.find_first_of('\0'));

    if(input_stream.fail()) {
        switch(input_stream.gcount()) {
            case 0:
                /* DO NOTHING. Evenly divisible by 4. */
                break;
            case 1:
            {
                /* Only one byte read */
                encoding_buffer[2] = 0;
                encoding_buffer[1] = 0;

                char firstsymbol = GetFirstSymbol(encoding_buffer);
                char secondsymbol = GetSecondSymbol(encoding_buffer);
                char thirdsymbol = GetThirdSymbol(encoding_buffer);
                char fourthsymbol = GetFourthSymbol(encoding_buffer);
                (void)(thirdsymbol);
                (void)(fourthsymbol);

                output.push_back(firstsymbol);
                output.push_back(secondsymbol);
                output.push_back(Base64Encoder::PADDING_CHAR);
                output.push_back(Base64Encoder::PADDING_CHAR);

                break;
            } case 2:
            {
                /* Only two bytes read */
                encoding_buffer[2] = 0;

                char firstsymbol = GetFirstSymbol(encoding_buffer);
                char secondsymbol = GetSecondSymbol(encoding_buffer);
                char thirdsymbol = GetThirdSymbol(encoding_buffer);
                char fourthsymbol = GetFourthSymbol(encoding_buffer);
                (void)(fourthsymbol);

                output.push_back(firstsymbol);
                output.push_back(secondsymbol);
                output.push_back(thirdsymbol);
                output.push_back(Base64Encoder::PADDING_CHAR);

                break;
            } case 3:
                /* DO NOTHING All three bytes read. */
                break;
            default:
                /* DO NOTHING */;
        }
    }

    input_stream.clear();

    return output;
}

std::string Base64Encoder::Encode(const std::string& str) const noexcept {
    (void)str;
    return std::string{};
}

std::string Base64Encoder::Execute(std::ifstream& file_input_stream) const noexcept {
    return Encode(file_input_stream);
}

std::string Base64Encoder::Execute(std::istream& input_stream) const noexcept {
    return Encode(input_stream);
}

std::string Base64Encoder::Execute(const std::string& str) const noexcept {
    return Encode(str);
}

/************************************************************************/
/* DECODER                                                              */
/************************************************************************/

const Base64Decoder::DecodingMap Base64Decoder::DecodingTable =
{
    { 'A', 0 },{ 'Q', 16 },{ 'g', 32 },{ 'w', 48 }
    ,{ 'B', 1 },{ 'R', 17 },{ 'h', 33 },{ 'x', 49 }
    ,{ 'C', 2 },{ 'S', 18 },{ 'i', 34 },{ 'y', 50 }
    ,{ 'D', 3 },{ 'T', 19 },{ 'j', 35 },{ 'z', 51 }
    ,{ 'E', 4 },{ 'U', 20 },{ 'k', 36 },{ '0', 52 }
    ,{ 'F', 5 },{ 'V', 21 },{ 'l', 37 },{ '1', 53 }
    ,{ 'G', 6 },{ 'W', 22 },{ 'm', 38 },{ '2', 54 }
    ,{ 'H', 7 },{ 'X', 23 },{ 'n', 39 },{ '3', 55 }
    ,{ 'I', 8 },{ 'Y', 24 },{ 'o', 40 },{ '4', 56 }
    ,{ 'J', 9 },{ 'Z', 25 },{ 'p', 41 },{ '5', 57 }
    ,{ 'K', 10 },{ 'a', 26 },{ 'q', 42 },{ '6', 58 }
    ,{ 'L', 11 },{ 'b', 27 },{ 'r', 43 },{ '7', 59 }
    ,{ 'M', 12 },{ 'c', 28 },{ 's', 44 },{ '8', 60 }
    ,{ 'N', 13 },{ 'd', 29 },{ 't', 45 },{ '0', 61 }
    ,{ 'O', 14 },{ 'e', 30 },{ 'u', 46 },{ '+', 62 }
    ,{ 'P', 15 },{ 'f', 31 },{ 'v', 47 },{ '/', 63 }
};

const char Base64Decoder::PADDING_CHAR = '=';

//First Byte is all 6 of first symbol and 2 most significant bits of second symbol.
unsigned char Base64Decoder::GetFirstByte(const std::vector<unsigned char>& decoding_buffer) const noexcept {
    auto first_iter = DecodingTable.find(decoding_buffer[0]);
    auto second_iter = DecodingTable.find(decoding_buffer[1]);

    int first_index;
    if(first_iter == DecodingTable.end()) {
        first_index = decoding_buffer[0];
    } else {
        first_index = (*first_iter).second;
    }

    int second_index;
    if(second_iter == DecodingTable.end()) {
        second_index = decoding_buffer[1];
    } else {
        second_index = (*second_iter).second;
    }

    first_index = (first_index & 0x3F) << 2;
    second_index = (second_index & 0x30) >> 4;
    auto result = static_cast<unsigned char>(first_index | second_index);
    return result;
}

//Second Byte is 4 least significant bits of second symbol and 4 most significant bits of third symbol.
unsigned char Base64Decoder::GetSecondByte(const std::vector<unsigned char>& decoding_buffer) const noexcept {
    auto second_iter = DecodingTable.find(decoding_buffer[1]);
    auto third_iter = DecodingTable.find(decoding_buffer[2]);

    int second_index;
    if(second_iter == DecodingTable.end()) {
        second_index = decoding_buffer[1];
    } else {
        second_index = (*second_iter).second;
    }

    int third_index;
    if(third_iter == DecodingTable.end()) {
        third_index = decoding_buffer[2];
    } else {
        third_index = (*third_iter).second;
    }
    second_index = (second_index & 0x0F) << 4;
    third_index = (third_index & 0x3C) >> 2;
    auto result = static_cast<unsigned char>(second_index | third_index);
    return result;
}

//Third Byte is 2 least significant bits of third symbol and all of fourth symbol.
unsigned char Base64Decoder::GetThirdByte(const std::vector<unsigned char>& decoding_buffer) const noexcept {
    auto third_iter = DecodingTable.find(decoding_buffer[2]);
    auto fourth_iter = DecodingTable.find(decoding_buffer[3]);

    int third_index;
    if(third_iter == DecodingTable.end()) {
        third_index = decoding_buffer[2];
    } else {
        third_index = (*third_iter).second;
    }

    int fourth_index;
    if(fourth_iter == DecodingTable.end()) {
        fourth_index = decoding_buffer[3];
    } else {
        fourth_index = (*fourth_iter).second;
    }

    third_index = (third_index & 0x03) << 6;
    fourth_index = fourth_index & 0x3F;
    auto result = static_cast<unsigned char>(third_index | fourth_index);
    return result;
}

std::string Base64Decoder::Decode(const std::string& file_path) const noexcept {
    std::string output;
    std::ifstream ifs;
    ifs.open(file_path.c_str(), std::ios_base::binary);
    try {
        output = Decode(ifs);
        ifs.close();
    } catch(...) {
        ifs.close();
    }
    return output;
}

std::string Base64Decoder::Decode(std::istream& input_stream) const noexcept {

    if(input_stream.fail()) return "";

    unsigned long file_size = 0;
    std::vector<unsigned char> decoding_buffer{ '\0', '\0', '\0', '\0' };

    input_stream.seekg(0, std::ios::end);
    file_size = static_cast<unsigned long>(input_stream.tellg());
    input_stream.seekg(0);

    std::string output(static_cast<unsigned long>(3 * std::ceil(file_size / 4.0)) + (file_size % 3), '\0');

    if(file_size == 0) {
        output.clear();
        input_stream.clear();
        return "";
    }

    while(input_stream.read(reinterpret_cast<char*>(decoding_buffer.data()), decoding_buffer.size())) {

        char firstbyte = GetFirstByte(decoding_buffer);
        char secondbyte = GetSecondByte(decoding_buffer);
        char thirdbyte = GetThirdByte(decoding_buffer);

        unsigned long s = static_cast<unsigned long>(3 * std::ceil(static_cast<unsigned long>(input_stream.tellg()) / 4.0));

        if(firstbyte != PADDING_CHAR) output[s - 3] = firstbyte;
        if(secondbyte != PADDING_CHAR) output[s - 2] = secondbyte;
        if(thirdbyte != PADDING_CHAR) output[s - 1] = thirdbyte;

        decoding_buffer[0] = '\0';
        decoding_buffer[1] = '\0';
        decoding_buffer[2] = '\0';
        decoding_buffer[3] = '\0';
    }

    //Erase extraneous null chars.
    output.erase(output.find_first_of('\0'));

    if(input_stream.fail()) {
        if(decoding_buffer[2] == PADDING_CHAR) {
            //Third character is PADDING_CHAR. Only one in decoding_buffer.
            char firstbyte = GetFirstByte(decoding_buffer);
            char secondbyte = GetSecondByte(decoding_buffer);
            char thirdbyte = GetThirdByte(decoding_buffer);
            output.push_back(firstbyte);
            (void)(secondbyte);
            (void)(thirdbyte);
        } else if(decoding_buffer[3] == PADDING_CHAR) {
            //Fourth character is PADDING_CHAR. Only two in decoding_buffer.
            char firstbyte = GetFirstByte(decoding_buffer);
            char secondbyte = GetSecondByte(decoding_buffer);
            char thirdbyte = GetThirdByte(decoding_buffer);
            output.push_back(firstbyte);
            output.push_back(secondbyte);
            (void)(thirdbyte);
        } else {
            /* DO NOTHING */
        }
    }
    input_stream.clear();

    return output;
}

std::string Base64Decoder::Execute(std::istream& input_stream) const noexcept {
    return Decode(input_stream);
}

std::string Base64Decoder::Execute(const std::string& file_path) const noexcept {
    return Decode(file_path);
}

}