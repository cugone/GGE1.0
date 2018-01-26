#pragma once

#include <string>
#include <vector>

class Matrix4;
class Vector4;
class Vector3;
class Vector2;
class IntVector4;
class IntVector3;
class IntVector2;
struct Vertex3D;
struct Vertex2D;
class Quaternion;
class Rgba;

namespace Net {
    class Message;
}

namespace FileUtils {

bool WriteBufferToFile(void* buffer, std::size_t size, const std::string& filePath);
bool ReadBufferFromFile(std::vector<unsigned char>& out_buffer, const std::string& filePath);

static constexpr uint32_t ENDIAN_CHECK = 0x01020304;

bool constexpr IsBigEndian();

enum eEndianness {
    LITTLE_ENDIAN,
    BIG_ENDIAN,
};

// 
eEndianness constexpr GetHostOrder();


//------------------------------------------------------------------------------
// BINARY STREAM TYPES
//------------------------------------------------------------------------------

class BinaryStream {
public:
    eEndianness stream_order;

    virtual std::size_t read_bytes(void* out_buffer, const std::size_t count) = 0;
    virtual std::size_t write_bytes(const void* buffer, const std::size_t size) const = 0;
    virtual ~BinaryStream() = 0;

    // I assume most basic types want to be endian aware
    // if you are not okay with that assumption - give this a better name.
    template <typename T>
    bool write(const T& v) const
    {
        return (write_bytes_endian_aware(&v, sizeof(v)) == sizeof(v));
    };


    // I assume most basic types want to be endian aware
    // if you are not okay with that assumption - give this a better name.
    template <typename T>
    bool read(T& v)
    {
        return (read_bytes_endian_aware(&v, sizeof(v)) == sizeof(v));
    };

    eEndianness HostEndianOrder() const;

    bool should_flip() const;;

    std::size_t write_bytes_endian_aware(const void* bytes, std::size_t count) const;

    std::size_t read_bytes_endian_aware(void* bytes, std::size_t count);

protected:
    void CopyReversed(unsigned char* copy, const void* bytes, std::size_t count) const;
private:

};

template<>
bool BinaryStream::write(const Matrix4& mat4) const;

template<>
bool BinaryStream::read(Matrix4& mat4);


template<>
bool BinaryStream::write(const Vector4& v) const;

template<>
bool BinaryStream::read(Vector4& v);

template<>
bool BinaryStream::write(const Vector3& v) const;

template<>
bool BinaryStream::read(Vector3& v);

template<>
bool BinaryStream::write(const Vector2& v) const;

template<>
bool BinaryStream::read(Vector2& v);

template<>
bool BinaryStream::write(const IntVector4& v) const;

template<>
bool BinaryStream::read(IntVector4& v);

template<>
bool BinaryStream::write(const IntVector3& v) const;

template<>
bool BinaryStream::read(IntVector3& v);

template<>
bool BinaryStream::write(const IntVector2& v) const;

template<>
bool BinaryStream::read(IntVector2& v);

template<>
bool BinaryStream::write(const std::string& s) const;

template<>
bool BinaryStream::read(std::string& s);

template<>
bool BinaryStream::write(const Quaternion& s) const;

template<>
bool BinaryStream::read(Quaternion& s);

template<>
bool BinaryStream::write(const Vertex3D& v) const;

template<>
bool BinaryStream::read(Vertex3D& v);

template<>
bool BinaryStream::write(const Vertex2D& v) const;

template<>
bool BinaryStream::read(Vertex2D& v);

template<>
bool BinaryStream::write(const Rgba& c) const;

template<>
bool BinaryStream::read(Rgba& c);

class FileBinaryStream : public BinaryStream
{
public:
    //----------------------------------------------------------------------------
    FileBinaryStream();

    //----------------------------------------------------------------------------
    virtual ~FileBinaryStream();

    //----------------------------------------------------------------------------
    bool open_for_read(const std::string& filename);

    //----------------------------------------------------------------------------
    bool open_for_write(const std::string& filename);

    //----------------------------------------------------------------------------
    void close();

    //----------------------------------------------------------------------------
    // BinaryStream Impl
    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    // read/write 'count' bytes.  Returns number of bytes actually read. 
    // will return 0 on failure. 
    virtual std::size_t read_bytes(void* out_buffer, const std::size_t count) override;

    //----------------------------------------------------------------------------
    virtual std::size_t write_bytes(const void* buffer, const std::size_t size) const override;

    //----------------------------------------------------------------------------
    bool is_open() const;




public:
    FILE* file_pointer;

};

}
