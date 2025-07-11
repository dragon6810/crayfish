#pragma once

#include "Material.h"
#include "ShaderFragmentLit.h"

class MaterialLit : public Material
{
private:
    ShaderFragmentLit frag = ShaderFragmentLit();
public:
    Eigen::Vector3f GetColor(void);

    void SetColor(Eigen::Vector3f color);
    
    Texture2d& Texture(void);

    virtual ShaderFragment* GetFragmentShader(void) override;
};