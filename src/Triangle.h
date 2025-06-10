#pragma once

#include <optional>

#include <Eigen/Dense>

#include "RenderFrame.h"

class Triangle
{
private:
    Eigen::Vector3f points[3];
    RenderFrame* rendertarget = NULL;
public:
    Triangle(void);
    Triangle(RenderFrame& rendertarget);

    static bool PointInTriangle(Eigen::Vector2f p, Eigen::Vector2f tri[3]);

    void SetRenderTarget(RenderFrame& rendertarget);

    void Draw(void);

    Eigen::Vector3f& operator[](int i);
};