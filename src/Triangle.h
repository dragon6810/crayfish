#pragma once

#include <optional>

#include <Eigen/Dense>

#include "RenderFrame.h"

class Triangle
{
private:
    Eigen::Vector4f points[3];
    RenderFrame* rendertarget = NULL;
    uint32_t color = 0xFFFFFFFF;
public:
    Triangle(void);
    Triangle(RenderFrame& rendertarget);

    float depths[3];

    static float TriangleArea(Eigen::Vector2f tri[3]);
    static float TriangleLerp(Eigen::Vector2f p, float vals[3], Eigen::Vector2f tri[3]);
    static bool PointInTriangle(Eigen::Vector2f p, Eigen::Vector2f tri[3]);

    void SetRenderTarget(RenderFrame& rendertarget);
    void SetColor(uint32_t color);
    void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    void Draw(void);

    Eigen::Vector4f& operator[](int i);
};