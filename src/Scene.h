#pragma once

#include "Camera.h"
#include "Model.h"
#include "RenderFrame.h"

class Scene
{
private:
    RenderFrame *rendertarget = NULL;
public:
    std::vector<Model> models;
    std::unique_ptr<Camera> camera;

    void SetRenderTarget(RenderFrame& rendertarget);

    void Render(const Camera* camera);
};