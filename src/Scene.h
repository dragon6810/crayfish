#pragma once

#include "SceneNodeCamera.h"
#include "SceneNodeModel.h"
#include "RenderFrame.h"

class Scene
{
private:
    RenderFrame *rendertarget = NULL;
public:
    std::vector<Model> models;
    std::unique_ptr<SceneNodeCamera> camera;
    std::vector<std::shared_ptr<Material>> materials;

    void SetRenderTarget(RenderFrame& rendertarget);

    void Render(SceneNodeCamera* camera);
};