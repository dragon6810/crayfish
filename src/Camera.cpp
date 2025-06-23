#include "Camera.h"

Camera::Camera(void)
{
    ComputeViewMatrix();
    ComputeProjectionMatrix();
}

void Camera::ComputeViewMatrix(void)
{
    int i, j;

    Eigen::Matrix3f rotate3;
    Eigen::Matrix4f translate, rotate, transform;

    translate = Eigen::Matrix4f::Identity();
    for(i=0; i<3; i++)
        translate(i, 3) = -this->position[i];
    
    rotate3 = this->rotation.toRotationMatrix();
    rotate = Eigen::Matrix4f::Identity();
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
            rotate(i, j) = rotate3(j, i);
    
    transform = rotate * translate;
    this->view = transform;
}

void Camera::ComputeProjectionMatrix(void)
{
    this->projection = Eigen::Matrix4f::Identity();
}

Eigen::Vector3f Camera::GetPosition(void) const
{
    return this->position;
}

Eigen::Quaternionf Camera::GetRotation(void) const
{
    return this->rotation;
}

float Camera::GetNearPlane(void) const
{
    return this->near;
}

float Camera::GetFarPlane(void) const
{
    return this->far;
}

Eigen::Matrix4f Camera::GetViewMatrix(void) const
{
    return this->view;
}

Eigen::Matrix4f Camera::GetProjectionMatrix(void) const
{
    return this->projection;
}

void Camera::SetPosition(Eigen::Vector3f position)
{
    this->position = position;
    this->ComputeViewMatrix();
}

void Camera::SetRotation(Eigen::Quaternionf rotation)
{
    this->rotation = rotation;
    this->ComputeViewMatrix();
}

void Camera::SetNearPlane(float near)
{
    this->near = near;
    this->ComputeProjectionMatrix();
}

void Camera::SetFarPlane(float far)
{
    this->far = far;
    this->ComputeProjectionMatrix();
}