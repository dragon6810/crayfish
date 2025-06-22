#pragma once

#include <stdint.h>
#include <vector>

class RenderFrame
{
public:
    uint64_t size[2];
    std::vector<uint32_t> pixels;
    std::vector<float> depths;

    RenderFrame(uint64_t width, uint64_t height, bool clear);

    void WritePng(const char* name);
};