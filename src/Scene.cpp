#include "Scene.h"

#include <sstream>

void Scene::SetRenderTarget(RenderFrame& rendertarget)
{
    this->rendertarget = &rendertarget;
}

void Scene::Render(SceneNodeCamera* camera)
{
    int i;

    long long start, stop;

    start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    for(i=0; i<this->models.size(); i++)
        this->models[i].Render(camera, this->rendertarget);

    stop = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    printf("scene render complete in %dms\n", stop - start);
}