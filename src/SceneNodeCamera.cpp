#include "SceneNodeCamera.h"

SceneNodeCamera::SceneNodeCamera(void)
{
    ComputeProjectionMatrix();
}

void SceneNodeCamera::ComputeProjectionMatrix(void)
{
    this->projection = Eigen::Matrix4f::Identity();
}

float SceneNodeCamera::GetNearPlane(void) const
{
    return this->near;
}

float SceneNodeCamera::GetFarPlane(void) const
{
    return this->far;
}

Eigen::Matrix4f SceneNodeCamera::GetViewMatrix(void) const
{
    return GetInvTransform();
}

Eigen::Matrix4f SceneNodeCamera::GetProjectionMatrix(void) const
{
    return this->projection;
}

void SceneNodeCamera::SetNearPlane(float near)
{
    this->near = near;
    this->ComputeProjectionMatrix();
}

void SceneNodeCamera::SetFarPlane(float far)
{
    this->far = far;
    this->ComputeProjectionMatrix();
}