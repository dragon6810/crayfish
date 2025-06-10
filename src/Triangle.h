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

    void SetRenderTarget(RenderFrame& rendertarget);

    void Draw(void);

    Eigen::Vector3f& operator[](int i);
};