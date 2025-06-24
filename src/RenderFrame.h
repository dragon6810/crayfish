#pragma once

#include <mutex>
#include <stdint.h>
#include <vector>

#include <Eigen/Dense>

class RenderFrame
{
public:
    uint64_t size[2];
    std::vector<Eigen::Vector4f> pixels;
    std::vector<float> depths;
    std::vector<std::unique_ptr<std::mutex>> locks;

    RenderFrame(uint64_t width, uint64_t height, bool clear);

    void WritePng(const char* name);
};