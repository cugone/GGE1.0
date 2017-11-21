#pragma once

#include "Engine/RHI/RHITypes.hpp"
#include "Engine/Math/IntVector2.hpp"

struct IDXGIDebug;

class RHIOutput;

class RHIInstance {
public:
    static RHIInstance* CreateInstance();
    static void DestroyInstance();

    RHIOutput* CreateOutput(const IntVector2& clientSize, const IntVector2& clientPosition = IntVector2::ZERO, const RHIOutputMode& outputMode = RHIOutputMode::WINDOWED);
protected:
	RHIInstance();
	~RHIInstance();

private:
    static RHIInstance* _instance;
    static IDXGIDebug* _debuggerInstance;
};