#pragma once

#include "Engine/RHI/DX11.hpp"

enum class BufferUsage : unsigned int;
enum class BufferBindUsage : unsigned int;

class IBuffer {
public:
	virtual ~IBuffer()=0;

    bool IsValid();
    ID3D11Buffer* GetDxBuffer();

protected:
    IBuffer();
    ID3D11Buffer* _dx_buffer;
private:
};