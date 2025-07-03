#include "Scene.h"

#include <type_traits>

SceneNode* Scene::GetNode(int node)
{
    if(node < 0 || node >= this->nodes.size())
    {
        fprintf(stderr, "fatal error in Scene::GetNode: node out of bounds\n");
        abort();
    }

    return this->nodes[node].get();
}

template <typename T>
std::vector<T*> Scene::GetNodesOfType(void)
{
    int i;

    std::vector<T*> res;
    std::unordered_map<std::type_index, std::vector<SceneNode*>>::iterator it;

    it = this->nodetypelookup.find(typeid(T));
    if(it == this->nodetypelookup.end())
        return res;

    for(i=0; i<it->second.size(); i++)
        res.push_back((T*) it->second[i]);

    return res;
}

template <typename T>
void Scene::AddNode(const T& node)
{
    if(!std::is_base_of<SceneNode, T>::value)
    {
        fprintf(stderr, "fatal error in Scene::AddNode: T must inherit from SceneNode\n");
        abort();
    }

    this->nodes.push_back(std::make_unique<SceneNode>(node));
    this->nodetypelookup[typeid(T)].push_back(this->nodes.back().get());

}

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