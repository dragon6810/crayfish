#pragma once

#include "Eigen/Dense"

class ShaderFragment
{
public:
    ShaderFragment(void);

    virtual Eigen::Vector3f Fragment(Eigen::Vector4f screenpos, Eigen::Vector3f worldpos, Eigen::Vector2f texcoord, Eigen::Vector3f normal);
};