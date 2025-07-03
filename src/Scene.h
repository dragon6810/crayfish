#pragma once

#include <typeindex>
#include <unordered_map>

#include "SceneNode.h"
#include "SceneNodeCamera.h"
#include "SceneNodeModel.h"
#include "RenderFrame.h"

class Scene
{
private:
    RenderFrame *rendertarget = NULL;

    std::vector<std::unique_ptr<SceneNode>> nodes;
    std::unordered_map<std::type_index, std::vector<SceneNode*>> nodetypelookup;
public:
    std::vector<Model> models;
    std::unique_ptr<SceneNodeCamera> camera;
    std::vector<std::shared_ptr<Material>> materials;

    SceneNode* GetNode(int node);
    template <typename T>
    std::vector<T*> GetNodesOfType(void);

    template <typename T>
    void AddNode(const T& node);

    void SetRenderTarget(RenderFrame& rendertarget);

    void Render(SceneNodeCamera* camera);
};