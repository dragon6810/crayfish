#pragma once

#include "SceneNodeCamera.h"

class SceneNodeCameraPerspective : public SceneNodeCamera
{
private:
    float fov = M_PI_2 * 2.0 / 3.0;
    float aspect = 1.0;

    void ComputeProjectionMatrix(void) override;
public:
    SceneNodeCameraPerspective(void);

    float GetFOV(void);
    float GetAspectRatio(void);

    void SetFOV(float fov);
    void SetAspectRatio(float aspect);
};