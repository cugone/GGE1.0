#include "Engine/RHI/RHITypes.hpp"

#include <type_traits>

#include "Engine/RHI/DX11.hpp"

BufferBindUsage operator|(const BufferBindUsage& a, const BufferBindUsage& b) {
    using underlying = std::underlying_type_t<BufferBindUsage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BufferBindUsage>(underlying_a | underlying_b);
}

BufferBindUsage operator&(const BufferBindUsage& a, const BufferBindUsage& b) {
    using underlying = std::underlying_type_t<BufferBindUsage>;
    auto underlying_a = static_cast<underlying>(a);
    auto underlying_b = static_cast<underlying>(b);
    return static_cast<BufferBindUsage>(underlying_a & underlying_b);
}

RHIOutputMode& operator++(RHIOutputMode& mode) {
    using IntType = typename std::underlying_type<RHIOutputMode>::type;
    mode = static_cast<RHIOutputMode>(static_cast<IntType>(mode) + 1);
    if(mode == RHIOutputMode::LAST_) {
        mode = RHIOutputMode::FIRST_;
    }
    return mode;
}

RHIOutputMode operator++(RHIOutputMode& mode, int) {
    RHIOutputMode result = mode;
    ++result;
    return result;
}
