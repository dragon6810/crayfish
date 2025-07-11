#include "MaterialLit.h"

Eigen::Vector3f MaterialLit::GetColor(void)
{
    return this->frag.color;
}

void MaterialLit::SetColor(Eigen::Vector3f color)
{
    this->frag.color = color;
}

Texture2d& MaterialLit::Texture(void)
{
    return this->frag.texture;
}

ShaderFragment* MaterialLit::GetFragmentShader(void)
{
    return &this->frag;
}