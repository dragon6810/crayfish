#include "ShaderFragmentLit.h"

Eigen::Vector3f ShaderFragmentLit::Fragment(Eigen::Vector4f screenpos, Eigen::Vector3f worldpos, Eigen::Vector2f texcoord, Eigen::Vector3f normal)
{
    Eigen::Vector4f texcol;

    normal.normalize();

    texcol = this->texture.Lookup(texcoord);

    return texcol.tail<3>();
    return Eigen::Vector3f(texcoord[0], texcoord[1], 0.0);

    return this->color * normal.dot(Eigen::Vector3f::UnitY()) * 0.5 + 0.5 * Eigen::Vector3f::Ones();
}