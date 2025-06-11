#pragma once

#include <Eigen/Dense>

class Model
{
private:
    Eigen::Vector3f position;
    Eigen::Quaternionf rotation;
    Eigen::Vector3f scale;

    Eigen::Matrix4f transform;

    void ComputeTransform(void);
};