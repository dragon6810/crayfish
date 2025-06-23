#pragma once

#include "Eigen/Dense"

class ShaderFragment
{
    virtual Eigen::Vector3f Fragment(Eigen::Vector4f screenpos, Eigen::Vector3f worldpos, Eigen::Vector3f uvw, Eigen::Vector3f normal);
};