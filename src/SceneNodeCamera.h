#pragma once

#include "SceneNode.h"

class SceneNodeCamera : public SceneNode
{
private:
    virtual void ComputeProjectionMatrix(void);
protected:
    float near = 1.0, far = 1024.0;
    Eigen::Matrix4f projection;
public:
    SceneNodeCamera(void);

    float GetNearPlane(void) const;
    float GetFarPlane(void) const;
    Eigen::Matrix4f GetViewMatrix(void) const;
    Eigen::Matrix4f GetProjectionMatrix(void) const;

    void SetNearPlane(float near);
    void SetFarPlane(float far);
};