#pragma once

#include <Eigen/Dense>

class Camera
{
private:
    Eigen::Vector3f position = Eigen::Vector3f(0, 0, 0);
    Eigen::Quaternionf rotation = Eigen::Quaternionf::Identity();

    Eigen::Matrix4f view;

    void ComputeViewMatrix(void);
    virtual void ComputeProjectionMatrix(void);
protected:
    float near = 1.0, far = 1024.0;
    Eigen::Matrix4f projection;
public:
    Camera(void);

    Eigen::Vector3f GetPosition(void);
    Eigen::Quaternionf GetRotation(void);
    float GetNearPlane(void);
    float GetFarPlane(void);
    Eigen::Matrix4f GetViewMatrix(void);
    Eigen::Matrix4f GetProjectionMatrix(void);

    void SetPosition(Eigen::Vector3f position);
    void SetRotation(Eigen::Quaternionf rotation);
    void SetNearPlane(float near);
    void SetFarPlane(float far);
};