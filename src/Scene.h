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
    std::vector<std::shared_ptr<Material>> materials;

    void SetRenderTarget(RenderFrame& rendertarget);

    void Render(const Camera* camera);
};