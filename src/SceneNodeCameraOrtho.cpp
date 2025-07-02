#include "SceneNodeCameraOrtho.h"

SceneNodeCameraOrtho::SceneNodeCameraOrtho(void) : SceneNodeCamera()
{
    this->ComputeProjectionMatrix();
}

void SceneNodeCameraOrtho::ComputeProjectionMatrix(void)
{
    this->projection = Eigen::Matrix4f::Identity();

    // Fit x and y to fit in -1 to +1
    this->projection(0, 0) = 2.0 / this->dimensions[0];
    this->projection(1, 1) = 2.0 / this->dimensions[1];

    // Make z fit in -1 to +1
    this->projection(2, 2) = -2.0 / (this->far - this->near);
    this->projection(2, 3) = -(this->far + this->near) / (this->far - this->near);
}

Eigen::Vector2f SceneNodeCameraOrtho::GetDimensions(void)
{
    return this->dimensions;
}

void SceneNodeCameraOrtho::SetDimensions(Eigen::Vector2f dimensions)
{
    this->dimensions = dimensions;
    this->ComputeProjectionMatrix();
}