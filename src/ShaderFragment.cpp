#include "ShaderFragment.h"

#include "Material.h"

ShaderFragment::ShaderFragment(void)
{
    
}

Eigen::Vector3f ShaderFragment::Fragment(Eigen::Vector4f screenpos, Eigen::Vector3f worldpos, Eigen::Vector2f texcoord, Eigen::Vector3f normal)
{
    return Eigen::Vector3f(1.0, 0.0, 1.0);
}