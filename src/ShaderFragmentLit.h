#pragma once

#include "ShaderFragment.h"

class ShaderFragmentLit : public ShaderFragment
{
public:
    Eigen::Vector3f color = Eigen::Vector3f(1.0, 1.0, 1.0);

    virtual Eigen::Vector3f Fragment(Eigen::Vector4f screenpos, Eigen::Vector3f worldpos, Eigen::Vector2f texcoord, Eigen::Vector3f normal) override;
};