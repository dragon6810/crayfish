#include "ShaderFragmentLit.h"

Eigen::Vector3f ShaderFragmentLit::Fragment(Eigen::Vector4f screenpos, Eigen::Vector3f worldpos, Eigen::Vector2f texcoord, Eigen::Vector3f normal)
{
    Eigen::Vector4f texcol;
    float light;

    normal.normalize();

    texcol = this->texture.Lookup(texcoord);
    light = normal.dot(Eigen::Vector3f::UnitY()) * 0.5 + 0.5;

    return color.cwiseProduct(texcol.tail<3>()) * light;
}