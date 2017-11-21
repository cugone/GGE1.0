#include "Engine/Renderer/IBuffer.hpp"

IBuffer::IBuffer() : _dx_buffer(nullptr) { /* DO NOTHING */ }

IBuffer::~IBuffer() { /* DO NOTHING */ }

bool IBuffer::IsValid() {
    return _dx_buffer != nullptr;
}
ID3D11Buffer* IBuffer::GetDxBuffer() {
    return _dx_buffer;
}
