#pragma once

#include "Camera.h"

class CameraOrtho : public Camera
{
private:
    Eigen::Vector2f dimensions = Eigen::Vector2f(0, 0);

    void ComputeProjectionMatrix(void) override;
public:
    Eigen::Vector2f GetDimensions(void);

    void SetDimensions(Eigen::Vector2f dimensions);
};