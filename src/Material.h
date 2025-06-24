#pragma once

#include "ShaderFragment.h"

class Material
{
public:
    virtual ShaderFragment* GetFragmentShader(void);
};