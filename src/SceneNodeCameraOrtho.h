#pragma once

#include "SceneNodeCamera.h"

class SceneNodeCameraOrtho : public SceneNodeCamera
{
private:
    Eigen::Vector2f dimensions = Eigen::Vector2f(0, 0);

    void ComputeProjectionMatrix(void) override;
public:
    SceneNodeCameraOrtho(void);

    Eigen::Vector2f GetDimensions(void);

    void SetDimensions(Eigen::Vector2f dimensions);
};