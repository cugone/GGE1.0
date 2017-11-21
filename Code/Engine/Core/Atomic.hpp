#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//--------------------------------------------------------------------
// Will return the result of the operation
__forceinline
unsigned int AtomicAdd(unsigned int volatile* ptr, unsigned int const value) {
    return static_cast<unsigned int>(::InterlockedAddNoFence((LONG volatile*)ptr, (LONG)value));
}

//--------------------------------------------------------------------
__forceinline
unsigned int AtomicIncrement(unsigned int* ptr) {
    return static_cast<unsigned int>(::InterlockedIncrementNoFence((LONG volatile*)ptr));
}

//--------------------------------------------------------------------
__forceinline
unsigned int AtomicDecrement(unsigned int* ptr) {
    return static_cast<unsigned int>(::InterlockedDecrementNoFence((LONG volatile*)ptr));
}

//--------------------------------------------------------------------
__forceinline
unsigned int CompareAndSet(unsigned int volatile* ptr, unsigned int const comparand, unsigned int const value) {
    /*
    uint const old_value = *ptr;
    if (old_value == comparand) {
    *ptr = value;
    }
    return old_value;
    */
    return ::InterlockedCompareExchange(ptr, value, comparand);
}

//--------------------------------------------------------------------
template <typename T>
__forceinline T* CompareAndSetPointer(T *volatile *ptr, T *comparand, T *value)
{
    return (T*)::InterlockedCompareExchangePointerNoFence((PVOID volatile*)ptr, (PVOID)value, (PVOID)comparand);
}

