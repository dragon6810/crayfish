#include "Scene.h"

void Scene::SetRenderTarget(RenderFrame& rendertarget)
{
    this->rendertarget = &rendertarget;
}

void Scene::Render(const Camera* camera)
{
    int i;

    for(i=0; i<this->models.size(); i++)
        this->models[i].Render(camera, this->rendertarget);
}