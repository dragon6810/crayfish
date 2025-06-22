#pragma once

#include <stdint.h>

class Random
{
public:
    static uint32_t UInt(uint64_t seed);
    static uint8_t UInt8(uint64_t seed);

    static uint32_t NextUInt(void);
    static uint8_t NextUInt8(void);
};