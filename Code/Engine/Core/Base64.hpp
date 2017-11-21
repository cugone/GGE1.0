#pragma once

#include <fstream>
#include <istream>
#include <map>
#include <string>
#include <vector>

namespace DataUtils {

class Base64Encoder {
public:
    Base64Encoder() = default;
    ~Base64Encoder() = default;

    std::string Execute(std::ifstream& file_input_stream) const noexcept;
    std::string Execute(std::istream& input_stream) const noexcept;
    std::string Execute(const std::string& str) const noexcept;

protected:
private:
    static const std::vector<char> EncodingTable;
    static const char PADDING_CHAR;

    int GetFirstSymbolIndex(const std::vector<unsigned char>& encoding_buffer) const noexcept;
    int GetSecondSymbolIndex(const std::vector<unsigned char>& encoding_buffer) const noexcept;
    int GetThirdSymbolIndex(const std::vector<unsigned char>& encoding_buffer) const noexcept;
    int GetFourthSymbolIndex(const std::vector<unsigned char>& encoding_buffer) const noexcept;
    unsigned char GetFirstSymbol(const std::vector<unsigned char>& encoding_buffer) const noexcept;
    unsigned char GetSecondSymbol(const std::vector<unsigned char>& encoding_buffer) const noexcept;
    unsigned char GetThirdSymbol(const std::vector<unsigned char>& encoding_buffer) const noexcept;
    unsigned char GetFourthSymbol(const std::vector<unsigned char>& encoding_buffer) const noexcept;
    std::string Encode(std::ifstream& file_path) const noexcept;
    std::string Encode(std::istream& input_stream) const noexcept;
    std::string Encode(const std::string& str) const noexcept;
};

class Base64Decoder {
public:
    Base64Decoder() = default;
    ~Base64Decoder() = default;
    unsigned char GetFirstByte(const std::vector<unsigned char>& decoding_buffer) const noexcept;
    unsigned char GetSecondByte(const std::vector<unsigned char>& decoding_buffer) const noexcept;
    unsigned char GetThirdByte(const std::vector<unsigned char>& decoding_buffer) const noexcept;
    std::string Decode(const std::string& file_path) const noexcept;
    std::string Decode(std::istream& input_stream) const noexcept;
    std::string Execute(std::istream& input_stream) const noexcept;
    std::string Execute(const std::string& file_path) const noexcept;
protected:
private:
    using DecodingMap = std::map<char, int>;
    static const char Base64Decoder::PADDING_CHAR;
    static const Base64Decoder::DecodingMap Base64Decoder::DecodingTable;

};

}