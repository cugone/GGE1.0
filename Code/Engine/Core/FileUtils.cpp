#include "Engine/Core/FileUtils.hpp"

#include <cstdio>

#include "Engine/EngineConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Quaternion.hpp"

#include "Engine/Renderer/Vertex2D.hpp"
#include "Engine/Renderer/Vertex3D.hpp"

namespace FileUtils {


bool WriteBufferToFile(void* buffer, std::size_t size, const std::string& filePath) {
    FILE* file = nullptr;
    errno_t errorCode = fopen_s(&file, filePath.c_str(), "wb");
    if(errorCode != 0) return false;
    std::size_t buffer_size = size;
    std::size_t numBytesWritten = fwrite(buffer, 1, buffer_size, file);
    bool isFileError = !ferror(file);
    fclose(file);

    //Bad write if numBytesWritten is less than buffer_size and there was an error.
    return !(numBytesWritten < buffer_size && isFileError);
}

bool ReadBufferFromFile(std::vector<unsigned char>& out_buffer, const std::string& filePath) {
    FILE* file = nullptr;
    errno_t errorCode = fopen_s(&file, filePath.c_str(), "rb");
    if(errorCode != 0) return false;

    fseek(file, 0, SEEK_END);
    std::size_t numBytes = ftell(file);
    fseek(file, 0, SEEK_SET);

    out_buffer.resize(numBytes);
    std::size_t numBytesRead = fread_s(out_buffer.data(), out_buffer.size(), 1, numBytes, file);
    bool isFileError = ferror(file) != 0;
    fclose(file);

    //Bad read if numBytesRead is less than file size and there was an error.
    return !(numBytesRead < numBytes && isFileError);
}

bool constexpr IsBigEndian() {
    return (reinterpret_cast<const char*>(&ENDIAN_CHECK))[0] == 0x01;
}
FileUtils::eEndianness constexpr GetHostOrder() {
    return IsBigEndian() ? FileUtils::eEndianness::BIG_ENDIAN : FileUtils::eEndianness::LITTLE_ENDIAN;
}
//------------------------------------------------------------------------------
// BINARY STREAM TYPES
//------------------------------------------------------------------------------

BinaryStream::~BinaryStream() {

}

template<>
bool BinaryStream::write(const Matrix4& mat4) const {
    auto ib = mat4.GetIBasis();
    auto jb = mat4.GetJBasis();
    auto kb = mat4.GetKBasis();
    auto tb = mat4.GetTBasis();
    if(!write(ib)) {
        return false;
    }
    if(!write(jb)) {
        return false;
    }
    if(!write(kb)) {
        return false;
    }
    if(!write(tb)) {
        return false;
    }
    return true;
}

template<>
bool BinaryStream::read(Matrix4& mat4) {
    Vector4 ib;
    if(!read(ib)) {
        return false;
    }

    Vector4 jb;
    if(!read(jb)) {
        return false;
    }

    Vector4 kb;
    if(!read(kb)) {
        return false;
    }

    Vector4 tb;
    if(!read(tb)) {
        return false;
    }
    mat4.SetIBasis(ib);
    mat4.SetJBasis(jb);
    mat4.SetKBasis(kb);
    mat4.SetTBasis(tb);
    return true;
}

FileUtils::eEndianness BinaryStream::HostEndianOrder() const {
    unsigned long long anywhere = 0x01; //Try to guarantee type is bigger than 1 byte
    unsigned char *b = (unsigned char*)(&anywhere);
    if(*b == 0x01) {
        return eEndianness::LITTLE_ENDIAN;
    } else {
        return eEndianness::BIG_ENDIAN;
    }
}
bool BinaryStream::should_flip() const {
    return stream_order != HostEndianOrder();
}
unsigned int BinaryStream::write_bytes_endian_aware(const void* bytes, unsigned int count) const {
    unsigned int bytes_written = 0;
    if(should_flip()) {
        unsigned char* copy = (unsigned char*)_alloca(count);
        CopyReversed(copy, bytes, count);
        bytes_written = write_bytes(copy, count);
    } else {
        bytes_written = write_bytes(bytes, count);
    }
    return bytes_written;
}
unsigned int BinaryStream::read_bytes_endian_aware(void* bytes, unsigned int count) {
    unsigned int bytes_read = read_bytes(bytes, count);
    if(should_flip()) {
        CopyReversed(reinterpret_cast<unsigned char*>(bytes), bytes, count);
    }
    return bytes_read;
}
void BinaryStream::CopyReversed(unsigned char* copy, const void* bytes, unsigned int count) const {
    copy = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(bytes));
    unsigned int last_index = count - 1u;
    unsigned int middle = count / 2u;
    for(unsigned int b = 0; b < middle; ++b) {
        std::swap(copy[b], copy[last_index - b]);
    }
}

template<>
bool BinaryStream::write(const Quaternion& q) const {
    return write(q.axis) && write(q.w);
}

template<>
bool BinaryStream::read(Quaternion& q) {
    return read(q.axis) && read(q.w);
}

template<>
bool BinaryStream::write(const Vector4& v) const {
    return write(v.x) && write(v.y) && write(v.z) && write(v.w);
}

template<>
bool BinaryStream::read(Vector4& v) {
    return read(v.x) && read(v.y) && read(v.z) && read(v.w);
}

template<>
bool BinaryStream::write(const Vector3& v) const {
    return write(v.x) && write(v.y) && write(v.z);
}

template<>
bool BinaryStream::read(Vector3& v) {
    return read(v.x) && read(v.y) && read(v.z);
}

template<>
bool BinaryStream::write(const Vector2& v) const {
    return write(v.x) && write(v.y);
}

template<>
bool BinaryStream::read(Vector2& v) {
    return read(v.x) && read(v.y);
}

template<>
bool BinaryStream::write(const IntVector4& v) const {
    return write(v.x) && write(v.y) && write(v.z) && write(v.w);
}

template<>
bool BinaryStream::read(IntVector4& v) {
    return read(v.x) && read(v.y) && read(v.z) && read(v.w);
}

template<>
bool BinaryStream::write(const IntVector3& v) const {
    return write(v.x) && write(v.y) && write(v.z);
}

template<>
bool BinaryStream::read(IntVector3& v) {
    return read(v.x) && read(v.y) && read(v.z);
}

template<>
bool BinaryStream::write(const IntVector2& v) const {
    return write(v.x) && write(v.y);
}

template<>
bool BinaryStream::read(IntVector2& v) {
    return read(v.x) && read(v.y);
}

template<>
bool BinaryStream::write(const Vertex3D& v) const {
    return write(v.position) &&
        write(v.color) &&
        write(v.texCoords) &&
        write(v.normal) &&
        write(v.tangent) &&
        write(v.bitangent) &&
        write(v.bone_weights) &&
        write(v.bone_indices);
}

template<>
bool BinaryStream::read(Vertex3D& v) {
    return read(v.position) &&
        read(v.color) &&
        read(v.texCoords) &&
        read(v.normal) &&
        read(v.tangent) &&
        read(v.bitangent) &&
        read(v.bone_weights) &&
        read(v.bone_indices);
}

template<>
bool BinaryStream::write(const Vertex2D& v) const {
    return write(v.position) && write(v.color) && write(v.texCoords);
}

template<>
bool BinaryStream::read(Vertex2D& v) {
    return read(v.position) && read(v.color) && read(v.texCoords);
}

template<>
bool BinaryStream::write(const Rgba& c) const {
    return write(c.r) && write(c.g) && write(c.b) && write(c.a);
}

template<>
bool BinaryStream::read(Rgba& c) {
    return read(c.r) && read(c.g) && read(c.b) && read(c.a);
}

template<>
bool BinaryStream::write(const std::string& s) const {
    std::size_t n_s = s.size();
    if(!write(n_s)) {
        return false;
    }
    return (write_bytes(s.c_str(), n_s) == n_s);
}

template<>
bool BinaryStream::read(std::string& s) {
    std::size_t n_s = 0;
    if(!read(n_s)) {
        return false;
    }
    s.resize(n_s);
    for(std::size_t i = 0; i < n_s; ++i) {
        char c = 0;
        if((read_bytes(&c, 1u) < 1u)) {
            return false;
        }
        s[i] = c;
    }
    return true;
}

FileBinaryStream::FileBinaryStream()
    : BinaryStream()
    , file_pointer(nullptr)
{
    stream_order = GetHostOrder();
}

FileBinaryStream::~FileBinaryStream() {
    close();
}

bool FileBinaryStream::open_for_read(const std::string& filename) {
    g_theFileLogger->LogFlush();
    ASSERT_OR_DIE(!is_open(), "FBS::open_for_read: FILE ALREADY OPEN.");
    errno_t errorCode = fopen_s(&file_pointer, filename.c_str(), "rb");
    if(errorCode != 0) return false;

    return is_open();
}

bool FileBinaryStream::open_for_write(const std::string& filename) {
    g_theFileLogger->LogFlush();
    ASSERT_OR_DIE(!is_open(), "FBS::open_for_write: FILE ALREADY OPEN.");
    errno_t errorCode = fopen_s(&file_pointer, filename.c_str(), "wb");
    if(errorCode != 0) return false;

    return is_open();
}
void FileBinaryStream::close() {
    if(is_open()) {
        fclose(file_pointer);
        file_pointer = nullptr;
    }
}
unsigned int FileBinaryStream::read_bytes(void* out_buffer, const unsigned int count) {
    unsigned int bytes_read = 0;
    if(is_open()) {
        bytes_read = (unsigned int)fread(out_buffer, 1, count, file_pointer);
        //bytes_read = (unsigned int)fread_s(out_buffer, count, 1, sizeof(out_buffer), file_pointer);
    }

    return bytes_read;
}
unsigned int FileBinaryStream::write_bytes(const void* out_buffer, const unsigned int count) const {
    unsigned int bytes_read = 0;
    if(is_open()) {
        bytes_read = (unsigned int)fwrite(out_buffer, 1, count, file_pointer);
    }

    return bytes_read;
}
bool FileBinaryStream::is_open() const {
    return file_pointer != nullptr;
}

}