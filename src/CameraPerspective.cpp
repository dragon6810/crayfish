#include "CameraPerspective.h"

CameraPerspective::CameraPerspective(void) : Camera()
{
    this->ComputeProjectionMatrix();
}

void CameraPerspective::ComputeProjectionMatrix(void)
{
    Eigen::Vector2f screen;

    this->projection = Eigen::Matrix4f::Identity();

    screen[1] = tanf(this->fov * 0.5) * this->near * 2;
    screen[0] = screen[1] * this->aspect;

    // Fit x and y to fit in -1 to +1
    this->projection(0, 0) = 2.0 * this->near / screen[0];
    this->projection(1, 1) = 2.0 * this->near / screen[1];

    // Make z fit in -1 to +1
    this->projection(2, 2) = (this->far + this->near) / (this->near - this->far);
    this->projection(2, 3) = (2.0 * this->far * this->near) / (this->near - this->far);

    // Set w to view z
    this->projection(3, 2) = -1.0;
    this->projection(3, 3) = 0.0;
}

float CameraPerspective::GetFOV(void)
{
    return this->fov;
}

float CameraPerspective::GetAspectRatio(void)
{
    return this->aspect;
}

void CameraPerspective::SetFOV(float fov)
{
    this->fov = fov;
    this->ComputeProjectionMatrix();
}

void CameraPerspective::SetAspectRatio(float aspect)
{
    this->aspect = aspect;
    this->ComputeProjectionMatrix();
}