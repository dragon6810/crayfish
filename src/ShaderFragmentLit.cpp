#include "ShaderFragmentLit.h"

Eigen::Vector3f ShaderFragmentLit::Fragment(Eigen::Vector4f screenpos, Eigen::Vector3f worldpos, Eigen::Vector3f uvw, Eigen::Vector3f normal)
{
    normal.normalize();

    return this->color * normal.dot(Eigen::Vector3f::UnitY()) * 0.5 + 0.5 * Eigen::Vector3f::Ones();
}