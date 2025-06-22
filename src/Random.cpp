#include "Random.h"

uint64_t seed = 1993;

uint32_t Random::UInt(uint64_t seed)
{
    seed = ((seed >> 16) ^ seed) * 0x45d9f3bu;
    seed = ((seed >> 16) ^ seed) * 0x45d9f3bu;
    seed = (seed >> 16) ^ seed;
    return seed;
}

uint8_t Random::UInt8(uint64_t seed)
{
    return (uint8_t) UInt(seed);
}

uint32_t Random::NextUInt(void)
{
    return seed = UInt(seed);
}

uint8_t Random::NextUInt8(void)
{
    return (uint8_t) NextUInt();
}