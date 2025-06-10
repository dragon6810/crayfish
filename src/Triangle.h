#pragma once

#include <optional>

#include <Eigen/Dense>

#include "RenderFrame.h"

class Triangle
{
private:
    Eigen::Vector3f points[3];
    RenderFrame* rendertarget = NULL;
    uint32_t color = 0xFFFFFFFF;
public:
    Triangle(void);
    Triangle(RenderFrame& rendertarget);

    static bool PointInTriangle(Eigen::Vector2f p, Eigen::Vector2f tri[3]);

    void SetRenderTarget(RenderFrame& rendertarget);
    void SetColor(uint32_t color);
    void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    void Draw(void);

    Eigen::Vector3f& operator[](int i);
};