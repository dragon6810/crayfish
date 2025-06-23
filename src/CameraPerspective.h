#pragma once

#include "Camera.h"

class CameraPerspective : public Camera
{
private:
    float fov = M_PI_2 * 2.0 / 3.0;
    float aspect = 1.0;

    void ComputeProjectionMatrix(void) override;
public:
    CameraPerspective(void);

    float GetFOV(void);
    float GetAspectRatio(void);

    void SetFOV(float fov);
    void SetAspectRatio(float aspect);
};